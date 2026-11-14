(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local fbxFile = (systemTools.getEnvVariable "USERPROFILE") + "\\Downloads\\Ammos.fbx"
	local texDir = projDir + "MaxData\\textures\\ammo\\"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local outFile = projDir + "MaxData\\EnemyBullet.max"
	local targetLength = 20.0

	FBXImporterSetParam "Animation" false
	importFile fbxFile #noPrompt using:FBXIMP

	local mm = Multimaterial numsubs:2 name:"M_EnemyBullet"
	local brass = DirectX_9_Shader name:"M_AmmoBrass"
	brass.effectFile = fxFile
	brass.g_albedo = openBitmap (texDir + "Ammo_Brass.png")
	local copper = DirectX_9_Shader name:"M_AmmoCopper"
	copper.effectFile = fxFile
	copper.g_albedo = openBitmap (texDir + "Ammo_Copper.png")
	mm.materialList[1] = brass
	mm.materialList[2] = copper

	local casing = getNodeByName "Cylinder.001"
	local tip = getNodeByName "Cone"
	for o in objects where o != casing and o != tip do delete o

	convertToPoly casing
	convertToPoly tip
	polyop.setFaceMatID casing #all 1
	local before = polyop.getNumFaces casing
	polyop.attach casing tip
	polyop.setFaceMatID casing #{(before + 1)..(polyop.getNumFaces casing)} 2
	casing.name = "EnemyBullet"
	resetXForm casing
	convertToPoly casing

	local bb = nodeGetBoundingBox casing (matrix3 1)
	local center = (bb[1] + bb[2]) * 0.5
	local s = targetLength / (bb[2].y - bb[1].y)
	for v = 1 to (polyop.getNumVerts casing) do
	(
		local p = ((polyop.getVert casing v) - center) * s
		polyop.setVert casing v [-p.x, -p.y, p.z]
	)
	casing.pivot = [0, 0, 0]
	addModifier casing (Uvwmap maptype:1)
	convertToPoly casing
	casing.material = mm

	local f = createFile (projDir + "MaxData\\bullet_report.txt")
	local bb2 = nodeGetBoundingBox casing (matrix3 1)
	format "scale:% bbox:% %\nfaces:%\n" s bb2[1] bb2[2] (polyop.getNumFaces casing) to:f
	close f

	saveMaxFile outFile quiet:true
)
