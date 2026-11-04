(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local objFile = projDir + "MaxData\\import\\blaster-g.obj"
	local texFile = projDir + "MaxData\\textures\\T_Blaster.png"
	local outFile = projDir + "MaxData\\Blaster.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local targetLength = 46.0

	importFile objFile #noPrompt

	local parts = for o in objects where superClassOf o == GeometryClass collect o
	local gun = convertToPoly parts[1]
	for i = 2 to parts.count do polyop.attach gun parts[i]
	gun.name = "Blaster"
	resetXForm gun
	convertToPoly gun

	local bb = nodeGetBoundingBox gun (matrix3 1)
	local size = bb[2] - bb[1]
	local longest = amax #(size.x, size.y, size.z)
	local s = targetLength / longest
	local center = (bb[1] + bb[2]) * 0.5
	for v = 1 to (polyop.getNumVerts gun) do
	(
		local p = ((polyop.getVert gun v) - center) * s
		polyop.setVert gun v [-p.x, -p.y, p.z]
	)

	local m = DirectX_9_Shader name:"M_Blaster"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile
	gun.material = m

	local f = createFile (projDir + "MaxData\\blaster_report.txt")
	format "size: % longest: % scale: %\n" size longest s to:f
	local bb2 = nodeGetBoundingBox gun (matrix3 1)
	format "bbox after: % %\n" bb2[1] bb2[2] to:f
	close f

	saveMaxFile outFile quiet:true
)
