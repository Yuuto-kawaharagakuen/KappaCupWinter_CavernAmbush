(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texDir = projDir + "MaxData\\textures\\effects\\"
	local outFile = projDir + "MaxData\\MuzzleFlash.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local starSize = 18.0
	local flameLength = 34.0
	local flameWidth = 13.0

	fn makeMat matName texPath fxFile =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = fxFile
		m.g_albedo = openBitmap texPath
		m.g_useNormalMap = false
		m.g_useMetallicSmoothMap = false
		m
	)

	fn bake obj =
	(
		convertToPoly obj
		obj.pivot = [0,0,0]
		resetXForm obj
		convertToPoly obj
		obj
	)

	local matStar = makeMat "M_MuzzleStar" (texDir + "FX_MuzzleStar.png") fxFile
	local matFlame = makeMat "M_MuzzleFlame" (texDir + "FX_MuzzleFlame.png") fxFile
	local mat = Multimaterial name:"M_MuzzleFlash" numsubs:2
	mat[1] = matStar
	mat[2] = matFlame

	local star = Plane length:starSize width:starSize lengthsegs:1 widthsegs:1 pos:[0,0,0] mapcoords:true
	rotate star (angleAxis 90 [1,0,0])
	bake star
	polyop.setFaceMatID star #{1..(polyop.getNumFaces star)} 1
	star.name = "MuzzleFlash"

	local flameA = Plane length:flameLength width:flameWidth lengthsegs:1 widthsegs:1 pos:[0, -flameLength * 0.5, 0] mapcoords:true
	local flameB = Plane length:flameLength width:flameWidth lengthsegs:1 widthsegs:1 pos:[0, -flameLength * 0.5, 0] mapcoords:true
	rotate flameB (angleAxis 90 [0,1,0])
	for o in #(flameA, flameB) do
	(
		bake o
		polyop.setFaceMatID o #{1..(polyop.getNumFaces o)} 2
		polyop.attach star o
	)
	star.material = mat

	saveMaxFile outFile quiet:true
)
