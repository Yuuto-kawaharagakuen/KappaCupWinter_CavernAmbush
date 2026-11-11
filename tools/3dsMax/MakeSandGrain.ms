(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\Ground079S_2K-JPG_Color.jpg"
	local outFile = projDir + "MaxData\\SandGrain.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_SandGrain"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local grain = GeoSphere radius:5 segs:1 baseType:2 pos:[0,0,0] mapcoords:true
	grain = convertToPoly grain
	grain.name = "SandGrain"
	seed 1111
	for v = 1 to (polyop.getNumVerts grain) do
	(
		local p = polyop.getVert grain v
		polyop.setVert grain v (p * (random 0.75 1.2))
	)
	addModifier grain (Uvwmap maptype:2 length:20 width:20 height:20)
	convertToPoly grain
	grain.pivot = [0,0,0]
	grain.material = m

	saveMaxFile outFile quiet:true
)
