(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local texDir = projDir + "MaxData\\textures\\props\\"
	local outFile = projDir + "MaxData\\Lantern.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	local lanternScale = 1.3
	local ropeLength = 100.0

	fn makeMat matName texPath fxFile =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = fxFile
		m.g_albedo = openBitmap texPath
		m.g_useNormalMap = false
		m.g_useMetallicSmoothMap = false
		m
	)

	fn bake obj =
	(
		convertToPoly obj
		obj.pivot = [0,0,0]
		resetXForm obj
		convertToPoly obj
		obj
	)

	fn scaleVerts obj rate =
	(
		for i = 1 to (polyop.getNumVerts obj) do polyop.setVert obj i ((polyop.getVert obj i) * rate)
		obj
	)

	local matMetal = makeMat "M_LanternMetal" (texDir + "Prop_LanternMetal.png") fxFile
	local matGlass = makeMat "M_LanternGlass" (texDir + "Prop_LanternGlass.png") fxFile
	local matRope = makeMat "M_LanternRope" (texDir + "Prop_Rope.png") fxFile

	local frame = bake (Cylinder radius:9 height:3 sides:12 heightsegs:1 capsegs:1 pos:[0,0,-13] mapcoords:true)
	frame.name = "Prop_Lantern"
	local parts = #()
	for i = 0 to 3 do
	(
		local a = 45.0 + i * 90.0
		append parts (Box length:1.4 width:1.4 height:20 pos:[7.0 * (cos a), 7.0 * (sin a), -10] mapcoords:true)
	)
	append parts (Cone radius1:9.5 radius2:2.5 height:8 sides:12 heightsegs:1 pos:[0,0,10] mapcoords:true)
	append parts (Cylinder radius:2.5 height:3 sides:8 heightsegs:1 capsegs:1 pos:[0,0,18] mapcoords:true)
	local ring = Torus radius1:3.5 radius2:0.7 segs:16 sides:6 pos:[0,0,24.5] mapcoords:true
	rotate ring (angleAxis 90 [1,0,0])
	append parts ring
	for o in parts do
	(
		bake o
		polyop.attach frame o
	)
	scaleVerts frame lanternScale
	frame.material = matMetal

	local glass = bake (Cylinder radius:6.5 height:20 sides:12 heightsegs:1 capsegs:1 pos:[0,0,-10] mapcoords:true)
	glass.name = "Prop_LanternGlass"
	scaleVerts glass lanternScale
	glass.material = matGlass

	local rope = bake (Cylinder radius:0.9 height:ropeLength sides:6 heightsegs:1 capsegs:1 pos:[0,0,0] mapcoords:true)
	rope.name = "Prop_LanternRope"
	rope.material = matRope

	saveMaxFile outFile quiet:true
)
