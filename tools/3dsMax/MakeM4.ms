(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local fbxFile = (systemTools.getEnvVariable "USERPROFILE") + "\\Downloads\\m4a4.fbx"
	global g_m4TexDir = projDir + "MaxData\\textures\\m4\\"
	global g_m4FxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local outFile = projDir + "MaxData\\M4.max"
	local targetLength = 80.0
	local eyePoint = [1334.0, 181.0, 1065.0]

	FBXImporterSetParam "Animation" false
	importFile fbxFile #noPrompt using:FBXIMP

	fn makeMaterial name albedo normal =
	(
		local m = DirectX_9_Shader name:name
		m.effectFile = ::g_m4FxFile
		m.g_albedo = openBitmap (::g_m4TexDir + albedo)
		if normal != undefined then
		(
			m.g_normal = openBitmap (::g_m4TexDir + normal)
			m.g_useNormalMap = true
		)
		m
	)

	local mm = Multimaterial numsubs:3 name:"M_M4"
	mm.materialList[1] = makeMaterial "M_M4Metal" "M4_Metal_Color.PNG" "M4_Metal_Normal.PNG"
	mm.materialList[2] = makeMaterial "M_M4Leather" "M4_Leather_Color.PNG" "M4_Leather_Normal.PNG"
	mm.materialList[3] = makeMaterial "M_M4Lens" "M4_Lens.png" undefined

	local parts = for o in objects where superClassOf o == GeometryClass collect o
	local gun = undefined
	for o in parts do
	(
		convertToPoly o
		local id = 1
		if o.name == "leather" then id = 2
		if o.name == "cam" then id = 3
		if gun == undefined then
		(
			gun = o
			polyop.setFaceMatID gun #all id
		)
		else
		(
			local before = polyop.getNumFaces gun
			polyop.attach gun o
			local after = polyop.getNumFaces gun
			polyop.setFaceMatID gun #{(before + 1)..after} id
		)
	)
	gun.name = "M4A4"
	gun.material = mm
	resetXForm gun
	convertToPoly gun

	local bb = nodeGetBoundingBox gun (matrix3 1)
	local s = targetLength / (bb[2].x - bb[1].x)
	for v = 1 to (polyop.getNumVerts gun) do
	(
		local p = ((polyop.getVert gun v) - eyePoint) * s
		polyop.setVert gun v [-p.y, p.x, p.z]
	)
	gun.pivot = [0, 0, 0]

	local f = createFile (projDir + "MaxData\\m4_report.txt")
	local bb2 = nodeGetBoundingBox gun (matrix3 1)
	format "scale:% bbox:% %\nfaces:%\n" s bb2[1] bb2[2] (polyop.getNumFaces gun) to:f
	close f

	saveMaxFile outFile quiet:true
)
