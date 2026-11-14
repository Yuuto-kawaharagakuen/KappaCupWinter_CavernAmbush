(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\effects\\FX_SandDust.png"
	local outFile = projDir + "MaxData\\SandDust.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_SandDust"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local board = Plane name:"SandDust" length:20 width:20 lengthsegs:1 widthsegs:1 pos:[0,0,0] mapcoords:true
	rotate board (angleAxis 90 [1, 0, 0])
	resetXForm board
	convertToPoly board
	board.pivot = [0,0,0]
	board.material = m

	saveMaxFile outFile quiet:true
)
