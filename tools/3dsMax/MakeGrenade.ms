(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texDir = projDir + "MaxData\\textures\\boss\\"
	local outFile = projDir + "MaxData\\Grenade.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	fn makeMat matName texPath fxFile =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = fxFile
		m.g_albedo = openBitmap texPath
		m.g_useNormalMap = false
		m.g_useMetallicSmoothMap = false
		m
	)

	local mm = Multimaterial numsubs:2 name:"M_Grenade"
	mm.materialList[1] = makeMat "M_GrenadeBody" (texDir + "Boss_Grenade.png") fxFile
	mm.materialList[2] = makeMat "M_GrenadeMetal" (texDir + "Boss_GearBlack.png") fxFile

	fn part obj matId =
	(
		obj.mapCoords = true
		convertToPoly obj
		polyop.setFaceMatID obj #all matId
		resetXForm obj
		convertToPoly obj
		obj
	)

	local body = Sphere radius:3.3 segs:16 pos:[0, 0, 0]
	body.scale = [1.0, 1.0, 1.25]
	part body 1
	body.name = "Grenade"
	polyop.attach body (part (Cylinder radius:1.1 height:2.4 sides:10 pos:[0, 0, 3.8]) 2)
	polyop.attach body (part (Box length:1.0 width:1.2 height:5.6 pos:[2.9, 0, -0.8]) 2)
	polyop.attach body (part (Torus radius1:1.2 radius2:0.25 segs:12 sides:6 pos:[-1.2, 0, 6.4]) 2)
	body.material = mm
	saveMaxFile outFile quiet:true
)
