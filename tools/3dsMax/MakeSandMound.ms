(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\Ground079S_2K-JPG_Color.jpg"
	local outFile = projDir + "MaxData\\SandMound.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	local m = DirectX_9_Shader name:"M_SandMound"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local mound = Sphere radius:50 segs:24 hemisphere:0.5 pos:[0,0,0]
	mound = convertToPoly mound
	mound.name = "SandMound"
	for v = 1 to (polyop.getNumVerts mound) do
	(
		local p = polyop.getVert mound v
		local r = sqrt (p.x * p.x + p.y * p.y) / 50.0
		local h = p.z * 0.45 * (1.0 + 0.15 * sin (atan2 p.y p.x * 5.0))
		local stretch = if p.y > 0 then 1.35 else 0.9
		polyop.setVert mound v [p.x * 0.8, p.y * stretch, h - 4.0]
	)
	addModifier mound (Uvwmap maptype:0 length:120 width:120)
	convertToPoly mound
	mound.material = m

	saveMaxFile outFile quiet:true
)
