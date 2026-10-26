(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	seed 20261023

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texDir = projDir + "MaxData\\textures\\"
	local outFile = projDir + "MaxData\\Stage1.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local CAVE_LEN = 4000.0
	local CAVE_WID = 2500.0
	local CAVE_HGT = 600.0
	local OVERLAP = 200.0

	fn makeMat matName texFile fxFile =
	(
		local m = undefined
		if doesFileExist fxFile do
		(
			try
			(
				m = DirectX_9_Shader name:matName
				m.effectFile = fxFile
				m.g_albedo = openBitmap texFile
			)
			catch
			(
				format "DirectX material failed (%): %\n" matName (getCurrentException())
				m = undefined
			)
		)
		if m == undefined do
		(
			m = StandardMaterial name:matName
			m.diffuseMap = Bitmaptexture filename:texFile
		)
		m
	)

	fn orientInward obj dir =
	(
		local tm = snapshotAsMesh obj
		local s = [0,0,0]
		for f = 1 to tm.numfaces do s += getFaceNormal tm f
		if (dot s dir) < 0 do addModifier obj (Normalmodifier flip:true)
	)

	fn addBoxUV obj =
	(
		addModifier obj (Uvwmap maptype:4 length:400 width:400 height:400)
	)

	fn makeWall nm w l ws ls p rotAxis rotAng inward noiseStr seedv =
	(
		local o = Plane name:nm width:w length:l widthsegs:ws lengthsegs:ls pos:[0,0,0]
		if rotAng != 0 do rotate o (angleaxis rotAng rotAxis)
		o.pos = p
		orientInward o inward
		addModifier o (Noisemodifier seed:seedv scale:250 fractal:true roughness:0.4 strength:[0,0,noiseStr])
		addBoxUV o
		o
	)

	fn makePillar p r seedv =
	(
		local o = Cylinder radius:r height:(CAVE_HGT + 150) sides:14 heightsegs:10 capsegs:1 pos:(p + [0,0,-50])
		addModifier o (Noisemodifier seed:seedv scale:120 fractal:true strength:[r * 0.6, r * 0.6, 0])
		addBoxUV o
		o
	)

	fn makeStalactite p r h seedv =
	(
		local o = Cone radius1:r radius2:0 height:h sides:10 heightsegs:5 pos:[0,0,0]
		rotate o (angleaxis 180 [1,0,0])
		o.pos = p
		addModifier o (Noisemodifier seed:seedv scale:60 strength:[r * 0.4, r * 0.4, 0])
		addBoxUV o
		o
	)

	fn bakeAtOrigin obj =
	(
		convertToPoly obj
		obj.pivot = [0,0,0]
		resetXForm obj
		convertToPoly obj
		obj
	)

	local matSand = makeMat "M_Sand" (texDir + "T_Sand.png") fxFile
	local matRock = makeMat "M_Rock" (texDir + "T_Rock.png") fxFile
	local matWood = makeMat "M_Wood" (texDir + "T_Wood.png") fxFile

	local halfL = CAVE_LEN * 0.5
	local halfW = CAVE_WID * 0.5
	local midH = CAVE_HGT * 0.5
	local wallH = CAVE_HGT + OVERLAP
	local spanL = CAVE_LEN + OVERLAP * 2
	local spanW = CAVE_WID + OVERLAP * 2

	local floorObj = Plane name:"Floor_Stage1" width:CAVE_LEN length:CAVE_WID widthsegs:40 lengthsegs:25 pos:[0,0,0]
	addModifier floorObj (Noisemodifier seed:3 scale:500 strength:[0,0,20])
	addBoxUV floorObj
	bakeAtOrigin floorObj
	floorObj.material = matSand

	local parts = #()
	append parts (makeWall "WallS" spanL wallH 44 8 [0, -halfW, midH] [1,0,0] 90 [0,1,0] 120 11)
	append parts (makeWall "WallN" spanL wallH 44 8 [0,  halfW, midH] [1,0,0] 90 [0,-1,0] 120 12)
	append parts (makeWall "WallW" wallH spanW 8 29 [-halfL, 0, midH] [0,1,0] 90 [1,0,0] 120 13)
	append parts (makeWall "WallE" wallH spanW 8 29 [ halfL, 0, midH] [0,1,0] 90 [-1,0,0] 120 14)
	append parts (makeWall "Ceiling" spanL spanW 44 29 [0, 0, CAVE_HGT] [0,0,1] 0 [0,0,-1] 150 15)

	append parts (makePillar [-800,  450, 0] 180 21)
	append parts (makePillar [ 300, -500, 0] 150 22)
	append parts (makePillar [1300,  650, 0] 200 23)

	for i = 1 to 12 do
	(
		local px = random (-halfL + 300) (halfL - 300)
		local py = random (-halfW + 250) (halfW - 250)
		local r = random 50 110
		local h = random 120 240
		append parts (makeStalactite [px, py, CAVE_HGT + 40] r h (100 + i))
	)

	local wallObj = bakeAtOrigin parts[1]
	wallObj.name = "Wall_Stage1"
	for i = 2 to parts.count do
	(
		local p = bakeAtOrigin parts[i]
		polyop.attach wallObj p
	)
	wallObj.material = matRock

	local gateObj = Box name:"Gate_Stage1" length:500 width:80 height:450 lengthsegs:1 widthsegs:1 heightsegs:1 pos:[halfL - 60, -350, 0]
	addBoxUV gateObj
	bakeAtOrigin gateObj
	gateObj.material = matWood

	local playerStart = Point name:"PlayerStart" pos:[-halfL + 300, 0, 0] size:120 box:true cross:false
	local hole = Point name:"Hole_Stage1" pos:[halfL - 150, 450, 0] size:150 box:true cross:true

	local root = Dummy name:"Stage1" pos:[0,0,0] boxsize:[200,200,200]
	for o in #(floorObj, wallObj, gateObj, playerStart, hole) do o.parent = root

	format "Floor faces: %\n" (polyop.getNumFaces floorObj)
	format "Wall faces: %\n" (polyop.getNumFaces wallObj)
	format "Material class: %\n" (classof matRock)

	saveMaxFile outFile quiet:true
	format "Saved: %\n" outFile
)
