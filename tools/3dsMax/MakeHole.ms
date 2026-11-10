(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texFile = projDir + "MaxData\\textures\\props\\T_Hole.png"
	local outFile = projDir + "MaxData\\Hole.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local radius = 120.0

	local m = DirectX_9_Shader name:"M_StageHole"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local hole = Cylinder radius:radius height:1 sides:48 heightsegs:1 capsegs:1 pos:[0,0,0]
	hole = convertToPoly hole
	hole.name = "StageHole"
	local removeFaces = for f = 1 to (polyop.getNumFaces hole) where (polyop.getFaceNormal hole f).z < 0.9 collect f
	polyop.deleteFaces hole (removeFaces as bitarray) delIsoVerts:true
	for v = 1 to (polyop.getNumVerts hole) do
	(
		local p = polyop.getVert hole v
		local rate = 1.0 + (noise3 [p.x / 45.0, p.y / 45.0, 2.4]) * 0.22
		polyop.setVert hole v [p.x * rate, p.y * rate, 0]
	)
	addModifier hole (Uvwmap maptype:0 length:320 width:320)
	convertToPoly hole
	hole.pivot = [0,0,0]
	hole.material = m

	saveMaxFile outFile quiet:true
)
