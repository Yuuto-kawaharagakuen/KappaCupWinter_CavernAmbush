(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	seed 20261027

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	global g_texDir = projDir + "MaxData\\textures\\"
	local outFile = projDir + "MaxData\\Stage1.max"
	global g_fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	global G_GRID_MIN = [-4400, -1800]
	global G_GRID_MAX = [3700, 1600]
	global G_GRID_STEP = 56.0
	global G_SCALE = 1.071
	global G_MAX_HEIGHT = 650.0
	global G_FALLOFF = 450.0
	global G_OUTSIDE_DEPTH = -250.0

	global g_caveEllipses = #(
		#([-2750, -150], [800, 700]),
		#([-200, 150], [1400, 1000]),
		#([2550, 0], [900, 800])
	)
	global g_caveCapsules = #(
		#([-2200, -150], [-1300, 100], 450),
		#([1000, 50], [1800, 0], 450),
		#([-3250, -500], [-3500, -1250], 210),
		#([-3500, -1250], [-4000, -1250], 210),
		#([-4000, -1250], [-4800, -1250], 250)
	)
	global G_TUNNEL_HALF_WIDTH = 95.0
	global G_TUNNEL_HEIGHT = 135.0
	global g_diveTunnels = #(
		#([-3380, -900], [-2600, -1100], [-1650, -480]),
		#([-1750, 380], [-1450, 950], [-950, 950]),
		#([700, 750], [1400, 750], [2150, 600]),
		#([400, -650], [1400, -700], [2100, -600])
	)
	global g_playerStart = [-3950, -1250]

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
		d += (noise3 [x / 700.0, y / 700.0, 0.5]) * 110.0
		d += (noise3 [x / 180.0, y / 180.0, 3.3]) * 30.0
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

	fn floorHeight x y =
	(
		(noise3 [x / 600.0, y / 600.0, 9.1]) * 25.0 + (noise3 [x / 150.0, y / 150.0, 4.2]) * 6.0
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
			h += (noise3 [x / 350.0, y / 350.0, 7.7]) * 90.0 * t
			h += (noise3 [x / 90.0, y / 90.0, 1.9]) * 15.0
			h
		)
	)

	fn ceilHeight x y =
	(
		local h = caveCeilHeight x y
		local t = tunnelSDF x y
		if t > 0 do h = amax h ((floorHeight x y) + G_TUNNEL_HEIGHT)
		h
	)

	fn sightBlocked a b =
	(
		local n = 120
		local blocked = false
		for i = 1 to (n - 1) while not blocked do
		(
			local p = a + (b - a) * (i as float / n)
			if ((ceilHeight p.x p.y) - (floorHeight p.x p.y)) < 150.0 do blocked = true
		)
		blocked
	)

	fn makeMat matName colorFile normalFile =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = g_fxFile
		m.g_albedo = openBitmap (g_texDir + colorFile)
		m.g_normal = openBitmap (g_texDir + normalFile)
		m.g_useNormalMap = true
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

	local matSand = makeMat "M_CaveSand" "Ground079S_2K-JPG_Color.jpg" "Ground079S_2K-JPG_NormalDX.jpg"
	local matWall = makeMat "M_CaveWall" "Rock061_2K-JPG_Color.jpg" "Rock061_2K-JPG_NormalDX.jpg"
	local matCeil = makeMat "M_CaveCeiling" "Rock060_2K-JPG_Color.jpg" "Rock060_2K-JPG_NormalDX.jpg"
	local matWood = DirectX_9_Shader name:"M_Wood"
	matWood.effectFile = g_fxFile
	matWood.g_albedo = openBitmap (g_texDir + "T_Wood.png")

	local floorObj = makeGrid "Floor_Stage1"
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
	floorObj.material = matSand

	local shellObj = makeGrid "Wall_Stage1"
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
	local pillarPos = #([-100, 400])
	for i = 1 to pillarPos.count do
	(
		local p = pillarPos[i]
		local r = random 140 200
		local top = (ceilHeight p.x p.y) + 120
		local o = Cylinder radius:r height:(top + 80) sides:16 heightsegs:12 capsegs:1 pos:[p.x, p.y, -80]
		addModifier o (Taper amount:-0.35 curve:1.2)
		addModifier o (Noisemodifier seed:(30 + i) scale:110 fractal:true strength:[r * 0.5, r * 0.5, 0])
		append rocks o
	)

	local placed = 0
	local tries = 0
	while placed < 26 and tries < 2000 do
	(
		tries += 1
		local px = random G_GRID_MIN.x G_GRID_MAX.x
		local py = random G_GRID_MIN.y G_GRID_MAX.y
		if (caveSDF px py) > 300.0 do
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

	local mounds = #([-2400, 350], [-900, -450], [600, -400], [1400, 250], [2800, -350])
	for i = 1 to mounds.count do
	(
		local p = mounds[i]
		local r = random 70 120
		local h = random 120 220
		local o = Cone radius1:r radius2:(r * 0.15) height:h sides:12 heightsegs:5 pos:[p.x, p.y, -30]
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
	local matShell = Multimaterial name:"M_CaveShell" numsubs:2
	matShell[1] = matWall
	matShell[2] = matCeil
	shellObj.material = matShell

	local desertMinX = -16000.0
	local desertMaxX = G_GRID_MIN.x + 20.0
	local desertMinY = -12000.0
	local desertMaxY = 9000.0
	local desertStep = 250.0
	local dw = desertMaxX - desertMinX
	local dl = desertMaxY - desertMinY
	local desertObj = Plane name:"Desert_Stage1" width:dw length:dl widthsegs:((dw / desertStep) as integer) lengthsegs:((dl / desertStep) as integer) pos:[(desertMinX + desertMaxX) * 0.5, (desertMinY + desertMaxY) * 0.5, 0]
	bakeAtOrigin desertObj
	for i = 1 to (polyop.getNumVerts desertObj) do
	(
		local v = polyop.getVert desertObj i
		local s = amax 0.0 (amin 1.0 ((G_GRID_MIN.x - v.x) / 1800.0))
		s = s * s * (3.0 - 2.0 * s)
		local dune = ((noise3 [v.x / 2600.0, v.y / 2600.0, 5.5]) + 0.6) * 280.0 + (noise3 [v.x / 900.0, v.y / 1500.0, 2.2]) * 90.0
		polyop.setVert desertObj i [v.x, v.y, (floorHeight v.x v.y) - 4.0 + dune * s]
	)
	scaleObj desertObj
	finishGroundMesh desertObj 300
	desertObj.material = matSand

	local barrier = Point name:"EntranceBarrier" pos:[(G_GRID_MIN.x + 60.0) * G_SCALE, -1250 * G_SCALE, 0] size:150 box:true cross:false

	local gateX = 2400.0
	while (caveSDF (gateX + 20.0) 0.0) > 80.0 do gateX += 20.0
	local gateObj = Box name:"Gate_Stage1" length:350 width:112 height:315 pos:[(gateX + 40.0) * G_SCALE, 0, -20]
	addModifier gateObj (Uvwmap maptype:4 length:200 width:200 height:200)
	bakeAtOrigin gateObj
	gateObj.material = matWood

	local holeY = 380.0
	local holeX = gateX - 220.0
	while (caveSDF holeX holeY) < 120.0 and holeY > 0.0 do holeY -= 20.0

	local playerStart = Point name:"PlayerStart" pos:[g_playerStart.x * G_SCALE, g_playerStart.y * G_SCALE, 60] size:120 box:true cross:false
	local hole = Point name:"Hole_Stage1" pos:[holeX * G_SCALE, holeY * G_SCALE, 0] size:150 box:true cross:true

	local root = Dummy name:"Stage1" pos:[0,0,0] boxsize:[200,200,200]
	for o in #(floorObj, shellObj, gateObj, desertObj, barrier, playerStart, hole) do o.parent = root

	local lightData = #(
		#("Light_Daylight", [-4250, -1250], 260),
		#("Light_Lamp", [-3100, 250], 200),
		#("Light_Lamp", [-2600, -150], 200),
		#("Light_Lamp", [-2780, -650], 200),
		#("Light_Lamp", [-1650, -150], 200),
		#("Light_Lamp", [-900, 650], 200),
		#("Light_Lamp", [-650, 50], 200),
		#("Light_Lamp", [100, -550], 200),
		#("Light_Lamp", [100, 50], 200),
		#("Light_Lamp", [1000, 250], 200),
		#("Light_Lamp", [1500, -300], 200),
		#("Light_Lamp", [2200, 50], 200),
		#("Light_Lamp", [2350, -520], 200),
		#("Light_Lamp", [3050, -200], 200)
	)
	local lightReport = #()
	for d in lightData do
	(
		local lp = d[2]
		local lz = amin ((floorHeight lp.x lp.y) + d[3]) ((ceilHeight lp.x lp.y) - 40.0)
		local l = Point name:d[1] pos:[lp.x * G_SCALE, lp.y * G_SCALE, lz * G_SCALE] size:80 box:false cross:true
		l.parent = root
		append lightReport #(d[1], lp, caveSDF lp.x lp.y, lz - (floorHeight lp.x lp.y))
	)

	local propClusters = #(
		#([-3100, 250], #(#("Prop_Crate", 0, 0, 20), #("Prop_WoodBarrel", -100, 110, 0))),
		#([-2780, -650], #(#("Prop_Drum", 0, 0, 0), #("Prop_DrumRusty", 65, 20, 40))),
		#([-2600, 50], #(#("Prop_Crate", 0, 0, 15), #("Prop_WoodBarrel", 110, -45, 0))),
		#([-2400, -150], #(#("Prop_SandbagStraight", 10, 340, 90), #("Prop_SandbagStraight", 10, 340, 90, 48), #("Prop_SandbagRound", 30, -110, -90), #("Prop_SandbagRound", 30, -110, -90, 48), #("Prop_SandbagStraight", 0, -580, 90), #("Prop_SandbagStraight", 0, -580, 90, 48))),
		#([-1650, -150], #(#("Prop_SandbagCorner", 0, 0, 45))),
		#([-1150, 150], #(#("Prop_Crate", 0, 0, -30))),
		#([-900, 700], #(#("Prop_SandbagCorner", 0, 0, -135))),
		#([-650, 50], #(#("Prop_Crate", 0, 0, 25), #("Prop_WoodBarrel", 125, 55, 0))),
		#([100, -550], #(#("Prop_UAZ", 0, 0, 98), #("Prop_JerrycanGreen", -260, 40, 15), #("Prop_JerrycanRed", -250, -5, -60))),
		#([100, 50], #(#("Prop_Crate", 0, 0, 10), #("Prop_Crate", 30, 135, -25))),
		#([550, 420], #(#("Prop_SandbagRound", 0, 0, -90))),
		#([900, -250], #(#("Prop_SandbagStraight", 0, 0, 20))),
		#([1000, 250], #(#("Prop_Crate", 0, 0, 40), #("Prop_WoodBarrel", -115, 35, 0))),
		#([1500, -300], #(#("Prop_Crate", 0, 0, 35))),
		#([2200, 50], #(#("Prop_Crate", 0, 0, -15), #("Prop_Drum", 115, 40, 0))),
		#([2350, -520], #(#("Prop_UAZ", 0, 0, 35), #("Prop_JerrycanGreen", 123, 86, 20), #("Prop_JerrycanRed", 100, 119, -15))),
		#([2500, 100], #(#("Prop_SandbagStraight", 0, 0, 90), #("Prop_SandbagStraight", 0, 0, 90, 48))),
		#([3050, -200], #(#("Prop_Crate", 0, 0, -20), #("Prop_Drum", 120, 60, 0)))
	)
	local propReport = #()
	for cluster in propClusters do
	(
		local center = cluster[1]
		append propReport #(center, caveSDF center.x center.y)
		for item in cluster[2] do
		(
			local wx = center.x * G_SCALE + item[2]
			local wy = center.y * G_SCALE + item[3]
			local wz = (floorHeight (wx / G_SCALE) (wy / G_SCALE)) * G_SCALE - 2.0 + (if item.count >= 5 then item[5] else 0.0)
			local p = Point name:item[1] pos:[0, 0, 0] size:60 box:true cross:false
			rotate p (angleAxis item[4] [0, 0, 1])
			p.pos = [wx, wy, wz]
			p.parent = root
		)
	)

	local rep = createFile (projDir + "MaxData\\stage_report.txt")
	for r in propReport do format "prop cluster % sdf:%\n" r[1] r[2] to:rep
	for r in lightReport do format "% % sdf:% height:%\n" r[1] r[2] r[3] r[4] to:rep
	for yy = 19 to -22 by -1 do
	(
		local line = ""
		for xx = -44 to 36 do line += (if (caveSDF (xx * 100.0) (yy * 100.0)) > 0 then "#" else (if (tunnelSDF (xx * 100.0) (yy * 100.0)) > 0 then "=" else "."))
		format "%\n" line to:rep
	)
	local guardPos = [-2200, 0]
	for s in #([-3950, -1250], [-3800, -1250], [-3650, -1250], [-3500, -1250], [-3450, -1100], [-3400, -950]) do
		format "sight guard->% blocked:%\n" s (sightBlocked guardPos s) to:rep
	format "Floor faces: %\n" (polyop.getNumFaces floorObj) to:rep
	format "Wall faces: %\n" (polyop.getNumFaces shellObj) to:rep
	format "Gate x: %  Hole: %, %\n" gateX holeX holeY to:rep

	saveMaxFile outFile quiet:true
	format "Saved: %\n" outFile to:rep
	close rep
)
