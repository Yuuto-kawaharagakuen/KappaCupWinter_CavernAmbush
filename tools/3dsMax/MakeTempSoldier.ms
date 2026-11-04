(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\T_Enemy.png"
	local outFile = projDir + "MaxData\\TempSoldier.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_TempSoldier"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local legL = Box length:22 width:20 height:80 pos:[-13,0,0]
	local legR = Box length:22 width:20 height:80 pos:[13,0,0]
	local body = Box length:30 width:52 height:70 pos:[0,0,80]
	local armL = Box length:16 width:14 height:62 pos:[-34,0,88]
	local armR = Box length:16 width:14 height:62 pos:[34,0,88]
	local head = Sphere radius:15 segs:16 pos:[0,0,165]
	local visor = Box length:6 width:22 height:7 pos:[0,-13,164]
	local rifle = Box length:50 width:6 height:8 pos:[24,-30,118]

	local soldier = convertToPoly body
	soldier.name = "TempSoldier"
	for p in #(legL, legR, armL, armR, head, visor, rifle) do polyop.attach soldier p
	addModifier soldier (Uvwmap maptype:4 length:40 width:40 height:40)
	convertToPoly soldier
	soldier.material = m

	saveMaxFile outFile quiet:true
)
