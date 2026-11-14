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
	local halfWidth = 60.0
	local frontLength = 55.0
	local backLength = 85.0
	local coreHeight = 20.0
	local rimHeight = 6.0
	local edgeDepth = -4.0
	local keepHeight = 2.5

	local m = DirectX_9_Shader name:"M_SandMound"
	m.effectFile = fxFile
	m.g_albedo = openBitmap texFile

	local mound = Plane name:"SandMound" width:(halfWidth * 2.0) length:(frontLength + backLength) widthsegs:36 lengthsegs:42 pos:[0, (backLength - frontLength) * 0.5, 0] mapcoords:true
	resetXForm mound
	mound = convertToPoly mound
	for v = 1 to (polyop.getNumVerts mound) do
	(
		local p = polyop.getVert mound v
		local nx = p.x / (halfWidth * 0.75)
		local ny = if p.y < 0 then p.y / (frontLength * 0.75) else p.y / (backLength * 0.75)
		local r = sqrt (nx * nx + ny * ny)
		local angle = atan2 ny nx
		local lumps = noise3 [p.x / 14.0, p.y / 14.0, 2.1]
		local grains = noise3 [p.x / 5.0, p.y / 5.0, 7.3]
		local core = (amax 0.0 (1.0 - r * r)) ^ 1.6 * coreHeight * (1.0 + 0.25 * lumps)
		local front = amax 0.0 (-ny / (amax r 0.001))
		local rimDistance = (r - 1.0) / 0.2
		local rim = exp (-1.0 * rimDistance * rimDistance) * rimHeight * (0.5 + 0.8 * front) * (1.0 + 0.6 * lumps)
		local crackNoise = abs (noise3 [p.x / 20.0, p.y / 20.0, 5.5])
		local crack = (amax 0.0 (1.0 - crackNoise / 0.07)) * 3.0 * (amax 0.0 (1.0 - abs (r - 0.5) / 0.45))
		local fade = amax 0.0 (amin 1.0 ((1.45 - r) / 0.35))
		local h = (core + rim - crack + grains * 1.2) * fade + edgeDepth * (1.0 - fade) + edgeDepth * 0.3
		polyop.setVert mound v [p.x, p.y, h]
	)
	local flatFaces = #{}
	for f = 1 to (polyop.getNumFaces mound) do
	(
		local isFlat = true
		for vi in (polyop.getFaceVerts mound f) do if (polyop.getVert mound vi).z > keepHeight do isFlat = false
		if isFlat do flatFaces[f] = true
	)
	polyop.deleteFaces mound flatFaces delIsoVerts:true
	mound.pivot = [0, 0, 0]
	mound.material = m

	saveMaxFile outFile quiet:true
)
