(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\T_Rock.png"
	local outFile = projDir + "MaxData\\Gun.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_Gun"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local body = Box length:24 width:6 height:8 pos:[0,-4,-4]
	local barrel = Cylinder radius:1.6 height:22 sides:12 pos:[0,-16,0] dir:[0,-1,0]
	local muzzle = Cylinder radius:2.4 height:4 sides:12 pos:[0,-36,0] dir:[0,-1,0]
	local tank = Cylinder radius:3 height:14 sides:12 pos:[0,-2,4] dir:[0,-1,0]
	local grip = Box length:5 width:4.5 height:12 pos:[0,6,-15] dir:[0,-0.26,0.97]
	local guard = Box length:8 width:1.5 height:1.5 pos:[0,1,-10]

	local gun = convertToPoly body
	gun.name = "Gun"
	for p in #(barrel, muzzle, tank, grip, guard) do polyop.attach gun p
	addModifier gun (Uvwmap maptype:4 length:20 width:20 height:20)
	convertToPoly gun
	gun.material = m

	saveMaxFile outFile quiet:true
)
