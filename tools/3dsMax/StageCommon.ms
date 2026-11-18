global g_texDir, g_fxFile
global G_GRID_MIN, G_GRID_MAX, G_GRID_STEP, G_SCALE, G_MAX_HEIGHT, G_FALLOFF, G_OUTSIDE_DEPTH, G_NOISE_SEED
global G_TUNNEL_HALF_WIDTH, G_TUNNEL_HEIGHT, G_CLIFF_NORMAL_Z, G_PLATEAU_HEADROOM
global G_PLATEAU_SLOPE = 220.0
global G_PLATEAU_FOOT = 70.0
global g_caveEllipses, g_caveCapsules, g_diveTunnels, g_plateaus

fn segDist p a b =
(
	local ab = b - a
	local ap = p - a
	local t = (ap.x * ab.x + ap.y * ab.y) / (ab.x * ab.x + ab.y * ab.y)
	t = amax 0.0 (amin 1.0 t)
	length (p - (a + ab * t))
)

fn caveSDF x y =
(
	local p = [x, y]
	local d = -1e9
	for e in g_caveEllipses do
	(
		local c = e[1]
		local r = e[2]
		local q = sqrt (((x - c.x) / r.x) ^ 2 + ((y - c.y) / r.y) ^ 2)
		d = amax d ((1.0 - q) * (amin r.x r.y))
	)
	for c in g_caveCapsules do
	(
		d = amax d (c[3] - (segDist p c[1] c[2]))
	)
	d += (noise3 [x / 700.0, y / 700.0, G_NOISE_SEED]) * 110.0
	d += (noise3 [x / 180.0, y / 180.0, G_NOISE_SEED + 2.8]) * 30.0
	d
)

fn tunnelSDF x y =
(
	local p = [x, y]
	local d = -1e9
	for t in g_diveTunnels do
	(
		for i = 1 to (t.count - 1) do d = amax d (G_TUNNEL_HALF_WIDTH - (segDist p t[i] t[i + 1]))
	)
	d
)

fn plateauHeight x y =
(
	local h = 0.0
	for pl in g_plateaus do
	(
		local c = pl[1]
		local r = pl[2]
		local q = sqrt (((x - c.x) / r.x) ^ 2 + ((y - c.y) / r.y) ^ 2)
		local d = (1.0 - q) * (amin r.x r.y)
		h = amax h (pl[3] * (amax 0.0 (amin 1.0 ((d + G_PLATEAU_FOOT) / G_PLATEAU_SLOPE))))
	)
	h
)

fn baseFloorHeight x y =
(
	(noise3 [x / 600.0, y / 600.0, G_NOISE_SEED + 8.6]) * 25.0 + (noise3 [x / 150.0, y / 150.0, G_NOISE_SEED + 3.7]) * 6.0
)

fn floorHeight x y =
(
	(baseFloorHeight x y) + (plateauHeight x y)
)

fn caveCeilHeight x y =
(
	local d = caveSDF x y
	if d <= 0 then
	(
		amax G_OUTSIDE_DEPTH (d * 1.5)
	)
	else
	(
		local t = amin 1.0 (d / G_FALLOFF)
		local h = G_MAX_HEIGHT * (t ^ 0.35)
		h += (noise3 [x / 350.0, y / 350.0, G_NOISE_SEED + 7.2]) * 90.0 * t
		h += (noise3 [x / 90.0, y / 90.0, G_NOISE_SEED + 1.4]) * 15.0
		h
	)
)

fn ceilHeight x y =
(
	local h = caveCeilHeight x y
	local t = tunnelSDF x y
	if t > 0 do h = amax h ((floorHeight x y) + G_TUNNEL_HEIGHT)
	local ph = plateauHeight x y
	if ph > 1.0 and (caveSDF x y) > 0 do h = amax h ((floorHeight x y) + ph + G_PLATEAU_HEADROOM)
	h
)

fn makeStageMat matName colorFile normalFile =
(
	local m = DirectX_9_Shader name:matName
	m.effectFile = g_fxFile
	m.g_albedo = openBitmap (g_texDir + colorFile)
	if normalFile != "" then
	(
		m.g_normal = openBitmap (g_texDir + normalFile)
		m.g_useNormalMap = true
	)
	else m.g_useNormalMap = false
	m.g_useMetallicSmoothMap = false
	m
)

fn bakeAtOrigin obj =
(
	convertToPoly obj
	obj.pivot = [0,0,0]
	resetXForm obj
	convertToPoly obj
	obj
)

fn makeGrid nm =
(
	local w = G_GRID_MAX.x - G_GRID_MIN.x
	local l = G_GRID_MAX.y - G_GRID_MIN.y
	local c = (G_GRID_MIN + G_GRID_MAX) * 0.5
	local o = Plane name:nm width:w length:l widthsegs:(w / G_GRID_STEP) lengthsegs:(l / G_GRID_STEP) pos:[c.x, c.y, 0]
	bakeAtOrigin o
)

fn scaleObj obj =
(
	for i = 1 to (polyop.getNumVerts obj) do polyop.setVert obj i ((polyop.getVert obj i) * G_SCALE)
	obj
)

fn finishMesh obj uvSize =
(
	local n = polyop.getNumFaces obj
	polyop.setFaceSmoothGroup obj #{1..n} 1
	addModifier obj (Uvwmap maptype:4 length:uvSize width:uvSize height:uvSize)
	convertToPoly obj
	obj
)

fn finishGroundMesh obj uvSize =
(
	local n = polyop.getNumFaces obj
	polyop.setFaceSmoothGroup obj #{1..n} 1
	local nv = polyop.getNumVerts obj
	polyop.setMapSupport obj 1 true
	polyop.setNumMapVerts obj 1 nv
	for i = 1 to nv do
	(
		local v = polyop.getVert obj i
		polyop.setMapVert obj 1 i [v.x / uvSize, v.y / uvSize, 0]
	)
	polyop.setNumMapFaces obj 1 n
	for f = 1 to n do polyop.setMapFace obj 1 f (polyop.getFaceVerts obj f)
	obj
)

fn buildFloor nm matSand matRock =
(
	local floorObj = makeGrid nm
	local floorDist = #()
	for i = 1 to (polyop.getNumVerts floorObj) do
	(
		local v = polyop.getVert floorObj i
		append floorDist (amax (caveSDF v.x v.y) (tunnelSDF v.x v.y))
		polyop.setVert floorObj i [v.x, v.y, floorHeight v.x v.y]
	)
	local floorDel = #{}
	for f = 1 to (polyop.getNumFaces floorObj) do
	(
		local inside = false
		for vi in (polyop.getFaceVerts floorObj f) do if floorDist[vi] > -200.0 do inside = true
		if not inside do floorDel[f] = true
	)
	polyop.deleteFaces floorObj floorDel delIsoVerts:true
	scaleObj floorObj
	finishGroundMesh floorObj 300
	for f = 1 to (polyop.getNumFaces floorObj) do
	(
		polyop.setFaceMatID floorObj f (if (polyop.getFaceNormal floorObj f).z < G_CLIFF_NORMAL_Z then 2 else 1)
	)
	local m = Multimaterial name:("M_" + nm) numsubs:2
	m[1] = matSand
	m[2] = matRock
	floorObj.material = m
	floorObj
)

fn buildShell nm matWall matCeil pillarPos stalactiteCount mounds =
(
	local shellObj = makeGrid nm
	local shellH = #()
	for i = 1 to (polyop.getNumVerts shellObj) do
	(
		local v = polyop.getVert shellObj i
		local h = ceilHeight v.x v.y
		append shellH h
		polyop.setVert shellObj i [v.x, v.y, h]
	)
	local shellDel = #{}
	for f = 1 to (polyop.getNumFaces shellObj) do
	(
		local keep = false
		for vi in (polyop.getFaceVerts shellObj f) do if shellH[vi] > (G_OUTSIDE_DEPTH + 1.0) do keep = true
		if not keep do shellDel[f] = true
	)
	polyop.deleteFaces shellObj shellDel delIsoVerts:true
	polyop.flipNormals shellObj #{1..(polyop.getNumFaces shellObj)}
	addModifier shellObj (Shell innerAmount:40 outerAmount:0)
	convertToPoly shellObj

	local rocks = #()
	for i = 1 to pillarPos.count do
	(
		local p = pillarPos[i][1]
		local r = pillarPos[i][2]
		local top = (ceilHeight p.x p.y) + 120
		local o = Cylinder radius:r height:(top + 80) sides:16 heightsegs:12 capsegs:1 pos:[p.x, p.y, -80]
		addModifier o (Taper amount:-0.35 curve:1.2)
		addModifier o (Noisemodifier seed:(30 + i) scale:110 fractal:true strength:[r * 0.5, r * 0.5, 0])
		append rocks o
	)

	local placed = 0
	local tries = 0
	while placed < stalactiteCount and tries < 3000 do
	(
		tries += 1
		local px = random G_GRID_MIN.x G_GRID_MAX.x
		local py = random G_GRID_MIN.y G_GRID_MAX.y
		if (caveSDF px py) > 300.0 and (plateauHeight px py) < 1.0 do
		(
			local ch = ceilHeight px py
			local h = amin (random 80 220) (ch - 320.0)
			if h > 60.0 do
			(
				local r = random 30 80
				local o = Cone radius1:r radius2:0 height:(h + 40) sides:10 heightsegs:6 pos:[px, py, ch + 40] dir:[0,0,-1]
				addModifier o (Noisemodifier seed:(100 + placed) scale:50 strength:[r * 0.35, r * 0.35, 0])
				append rocks o
				placed += 1
			)
		)
	)

	for i = 1 to mounds.count do
	(
		local p = mounds[i][1]
		local r = mounds[i][2]
		local h = mounds[i][3]
		local o = Cone radius1:r radius2:(r * 0.15) height:h sides:12 heightsegs:5 pos:[p.x, p.y, (floorHeight p.x p.y) - 30]
		addModifier o (Noisemodifier seed:(200 + i) scale:60 strength:[r * 0.35, r * 0.35, 10])
		append rocks o
	)

	for o in rocks do
	(
		bakeAtOrigin o
		polyop.attach shellObj o
	)
	scaleObj shellObj
	for f = 1 to (polyop.getNumFaces shellObj) do
	(
		polyop.setFaceMatID shellObj f (if (polyop.getFaceNormal shellObj f).z < -0.75 then 2 else 1)
	)
	finishMesh shellObj 350
	local matShell = Multimaterial name:("M_" + nm) numsubs:2
	matShell[1] = matWall
	matShell[2] = matCeil
	shellObj.material = matShell
	shellObj
)

fn makeDoor nm centerRaw widthRaw matDoor =
(
	local door = Box name:nm length:(widthRaw * G_SCALE) width:60 height:340 pos:[centerRaw.x * G_SCALE, centerRaw.y * G_SCALE, (floorHeight centerRaw.x centerRaw.y) * G_SCALE - 20]
	addModifier door (Uvwmap maptype:4 length:200 width:200 height:200)
	bakeAtOrigin door
	door.material = matDoor
	door
)

fn addStagePoint nm rawPos zOffset root =
(
	local p = Point name:nm pos:[rawPos.x * G_SCALE, rawPos.y * G_SCALE, ((floorHeight rawPos.x rawPos.y) + zOffset) * G_SCALE] size:120 box:true cross:false
	p.parent = root
	p
)

fn addStageLights lightData root =
(
	for d in lightData do
	(
		local lp = d[2]
		local lz = amin ((floorHeight lp.x lp.y) + d[3]) ((ceilHeight lp.x lp.y) - 40.0)
		local l = Point name:d[1] pos:[lp.x * G_SCALE, lp.y * G_SCALE, lz * G_SCALE] size:80 box:false cross:true
		l.parent = root
	)
)

fn writeStageMap rep stepRaw =
(
	local y = G_GRID_MAX.y
	while y >= G_GRID_MIN.y do
	(
		local line = ""
		local x = G_GRID_MIN.x
		while x <= G_GRID_MAX.x do
		(
			local c = "."
			if (caveSDF x y) > 0 then
			(
				c = if (plateauHeight x y) > 1.0 then "^" else "#"
			)
			else if (tunnelSDF x y) > 0 then c = "="
			line += c
			x += stepRaw
		)
		format "%\n" line to:rep
		y -= stepRaw
	)
)

fn writeClearance rep label rawPos =
(
	format "% % floor:% ceil:% clearance:% caveSDF:%\n" label rawPos (floorHeight rawPos.x rawPos.y) (ceilHeight rawPos.x rawPos.y) ((ceilHeight rawPos.x rawPos.y) - (floorHeight rawPos.x rawPos.y)) (caveSDF rawPos.x rawPos.y) to:rep
)
