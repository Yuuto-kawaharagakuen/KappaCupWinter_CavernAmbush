(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\T_Sand.png"
	local outFile = projDir + "MaxData\\SandBullet.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_SandBullet"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local ball = GeoSphere name:"SandBullet" radius:8 segs:3 pos:[0,0,0]
	addModifier ball (Noisemodifier seed:7 scale:6 strength:[3,3,3])
	addModifier ball (Uvwmap maptype:2)
	convertToPoly ball
	ball.material = m

	saveMaxFile outFile quiet:true
)
