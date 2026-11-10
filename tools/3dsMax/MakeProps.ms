(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local dl = (systemTools.getEnvVariable "USERPROFILE") + "\\Downloads\\"
	global g_propTexDir = projDir + "MaxData\\textures\\props\\"
	global g_propFxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	global g_propTexMap = #(
		#("TexturesCom_Barrels0031_1_M.jpg", "Prop_Drum_A.jpg"),
		#("TexturesCom_Barrels0031_3_S.jpg", "Prop_Drum_B.jpg"),
		#("TexturesCom_Barrels0036_2_seamless_S.jpg", "Prop_DrumRusty_A.jpg"),
		#("TexturesCom_JerryCan0003_1_M.jpg", "Prop_JerryGreen_A.jpg"),
		#("TexturesCom_JerryCan0003_3_M.jpg", "Prop_JerryGreen_B.jpg"),
		#("TexturesCom_JerryCan0001_1_M.jpg", "Prop_JerryRed_A.jpg")
	)
	local outFile = projDir + "MaxData\\Props.max"
	local rep = createFile (projDir + "MaxData\\props_report.txt")

	fn makeShader matName texPath =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = ::g_propFxFile
		m.g_albedo = openBitmap texPath
		m.g_useNormalMap = false
		m.g_useMetallicSmoothMap = false
		m
	)

	fn texOf m =
	(
		local result = undefined
		for t in (getClassInstances BitmapTexture target:m) while result == undefined do
		(
			local base = filenameFromPath t.filename
			for e in ::g_propTexMap where e[1] == base do result = e[2]
		)
		result
	)

	fn convertMaterial m prefix =
	(
		if classOf m == Multimaterial then
		(
			local mm = Multimaterial numsubs:m.numsubs name:("M_" + prefix)
			mm.materialIDList = m.materialIDList
			local fallback = undefined
			for i = 1 to m.numsubs where m.materialList[i] != undefined and fallback == undefined do fallback = texOf m.materialList[i]
			for i = 1 to m.numsubs do
			(
				local t = if m.materialList[i] != undefined then texOf m.materialList[i] else undefined
				if t == undefined then t = fallback
				mm.materialList[i] = makeShader ("M_" + prefix + "_" + (i as string)) (::g_propTexDir + t)
			)
			mm
		)
		else
		(
			makeShader ("M_" + prefix) (::g_propTexDir + (texOf m))
		)
	)

	fn normalizeProp o scaleRate =
	(
		resetXForm o
		convertToPoly o
		local bb = nodeGetBoundingBox o (matrix3 1)
		local base = [(bb[1].x + bb[2].x) * 0.5, (bb[1].y + bb[2].y) * 0.5, bb[1].z]
		for v = 1 to (polyop.getNumVerts o) do
		(
			polyop.setVert o v (((polyop.getVert o v) - base) * scaleRate)
		)
		o.pivot = [0, 0, 0]
		o
	)

	fn beginFaceMap o =
	(
		polyop.setMapSupport o 1 true
		local total = 0
		for f = 1 to (polyop.getNumFaces o) do total += polyop.getFaceDeg o f
		polyop.setNumMapVerts o 1 total keep:false
	)

	fn setFaceUV o f uvs idx =
	(
		local ids = #()
		for i = 1 to uvs.count do
		(
			polyop.setMapVert o 1 (idx + i - 1) [uvs[i].x, uvs[i].y, 0]
			append ids (idx + i - 1)
		)
		polyop.setMapFace o 1 f ids
		idx + uvs.count
	)

	fn projectFace o vs axisA axisB uMin uMax vMin vMax =
	(
		local minA = 1e9
		local maxA = -1e9
		local minB = 1e9
		local maxB = -1e9
		for v in vs do
		(
			local p = polyop.getVert o v
			local a = dot p axisA
			local b = dot p axisB
			if a < minA then minA = a
			if a > maxA then maxA = a
			if b < minB then minB = b
			if b > maxB then maxB = b
		)
		for v in vs collect
		(
			local p = polyop.getVert o v
			[uMin + (uMax - uMin) * ((dot p axisA) - minA) / (maxA - minA), vMin + (vMax - vMin) * ((dot p axisB) - minB) / (maxB - minB)]
		)
	)

	fn metalFaceUV o vs n =
	(
		local cmToUV = 0.99 / 135.0
		local count = vs.count
		local longest = [0, 0, 1]
		local longestLen = 0.0
		for i = 1 to count do
		(
			local d = (polyop.getVert o vs[(mod i count) as integer + 1]) - (polyop.getVert o vs[i])
			if (length d) > longestLen then
			(
				longestLen = length d
				longest = normalize d
			)
		)
		local axisB = longest
		local axisA = normalize (cross n axisB)
		local minA = 1e9
		local maxA = -1e9
		local minB = 1e9
		local maxB = -1e9
		for v in vs do
		(
			local p = polyop.getVert o v
			local a = dot p axisA
			local b = dot p axisB
			if a < minA then minA = a
			if a > maxA then maxA = a
			if b < minB then minB = b
			if b > maxB then maxB = b
		)
		local scaleA = amin cmToUV (0.24 / (maxA - minA))
		local scaleB = amin cmToUV (0.99 / (maxB - minB))
		local startU = 0.755 + (random 0.0 1.0) * (0.24 - (maxA - minA) * scaleA)
		local startV = 0.005 + (random 0.0 1.0) * (0.99 - (maxB - minB) * scaleB)
		for v in vs collect
		(
			local p = polyop.getVert o v
			[startU + ((dot p axisA) - minA) * scaleA, startV + ((dot p axisB) - minB) * scaleB]
		)
	)

	fn mapCrateUV o =
	(
		beginFaceMap o
		seed 4217
		local bodyArea = 5000.0
		local up = [0, 0, 1]
		local axisU = undefined
		for f = 1 to (polyop.getNumFaces o) while axisU == undefined do
		(
			local n = polyop.getFaceNormal o f
			if (polyop.getFaceArea o f) > bodyArea and (abs n.z) < 0.1 then axisU = normalize (cross up n)
		)
		local idx = 1
		for f = 1 to (polyop.getNumFaces o) do
		(
			local vs = polyop.getFaceVerts o f
			local n = polyop.getFaceNormal o f
			local uvs = undefined
			if (polyop.getFaceArea o f) > bodyArea then
			(
				local isCap = (abs n.z) > 0.5
				local axisA = if isCap then axisU else (normalize (cross up n))
				local axisB = if isCap then (cross up axisU) else up
				uvs = projectFace o vs axisA axisB 0.002 0.748 0.002 0.748
			)
			else
			(
				uvs = metalFaceUV o vs n
			)
			idx = setFaceUV o f uvs idx
		)
	)

	fn mapBarrelUV o =
	(
		beginFaceMap o
		local bb = nodeGetBoundingBox o (matrix3 1)
		local height = bb[2].z
		local seamAngle = undefined
		for f = 1 to (polyop.getNumFaces o) while seamAngle == undefined do
		(
			local n = polyop.getFaceNormal o f
			if (polyop.getFaceDeg o f) == 4 and (abs n.z) < 0.01 then seamAngle = (atan2 n.y n.x) + 15.0
		)
		local idx = 1
		for f = 1 to (polyop.getNumFaces o) do
		(
			local vs = polyop.getFaceVerts o f
			local uvs = #()
			if vs.count >= 8 then
			(
				local radius = 0.0
				for v in vs do
				(
					local p = polyop.getVert o v
					radius = amax radius (sqrt (p.x * p.x + p.y * p.y))
				)
				for v in vs do
				(
					local p = polyop.getVert o v
					append uvs [0.875 + p.x / radius * 0.12, 0.125 + p.y / radius * 0.12]
				)
			)
			else
			(
				local c = polyop.getFaceCenter o f
				local centerAngle = mod ((atan2 c.y c.x) - seamAngle) 360.0
				if centerAngle < 0.0 then centerAngle += 360.0
				for v in vs do
				(
					local p = polyop.getVert o v
					local delta = mod ((atan2 p.y p.x) - seamAngle - centerAngle) 360.0
					if delta > 180.0 then delta -= 360.0
					if delta < -180.0 then delta += 360.0
					local angleValue = amax 0.0 (amin 360.0 (centerAngle + delta))
					append uvs [0.001 + angleValue / 360.0 * 0.748, 0.005 + p.z / height * 0.99]
				)
			)
			idx = setFaceUV o f uvs idx
		)
	)

	fn importParts fbx =
	(
		local before = objects as array
		FBXImporterSetParam "Animation" false
		FBXImporterSetParam "Skin" false
		importFile fbx #noPrompt using:FBXIMP
		for o in objects where (findItem before o) == 0 and superClassOf o == GeometryClass collect o
	)

	local keep = #()

	local woodParts = importParts (dl + "wooden+box+and+barrel (1).fbx")
	for o in woodParts do
	(
		local isCrate = (o.name == "wooden box and barrel.001")
		convertToPoly o
		o.name = if isCrate then "Prop_Crate" else "Prop_WoodBarrel"
		o.material = makeShader ("M_" + o.name) (g_propTexDir + o.name + ".png")
		normalizeProp o (if isCrate then 0.5 else 0.43)
		if isCrate then mapCrateUV o else mapBarrelUV o
		append keep o
	)

	local canParts = importParts (dl + "Jerrycans+and+barrels+old+and+rusty.FBX\\Jerrycans and barrels old and rusty.FBX\\Jerrycans_and_barrels_old_and_rusty.fbx")
	for o in canParts do
	(
		local newName = case o.name of
		(
			"can": "Prop_Drum"
			"Rusty can": "Prop_DrumRusty"
			"jerrycan_green": "Prop_JerrycanGreen"
			"jerrycan_red": "Prop_JerrycanRed"
			default: undefined
		)
		if newName == undefined then continue
		convertToPoly o
		local mat = convertMaterial o.material newName
		o.name = newName
		normalizeProp o (if (findString newName "Jerrycan") != undefined then 1.5 else 1.0)
		o.material = mat
		if classOf mat == Multimaterial then
		(
			local firstId = mat.materialIDList[1]
			local fixedNum = 0
			for f = 1 to (polyop.getNumFaces o) do
			(
				if (findItem mat.materialIDList (polyop.getFaceMatID o f)) == 0 then
				(
					polyop.setFaceMatID o #{f} firstId
					fixedNum += 1
				)
			)
			format "% ids:% fixed:%\n" newName mat.materialIDList fixedNum to:rep
		)
		append keep o
	)

	local uazParts = importParts (dl + "UAZfbx(1)\\UAZfbx.fbx")
	local uaz = undefined
	for o in uazParts do
	(
		convertToPoly o
		if uaz == undefined then uaz = o else polyop.attach uaz o
	)
	uaz.name = "Prop_UAZ"
	normalizeProp uaz 0.82
	polyop.setFaceMatID uaz #all 1
	uaz.material = makeShader "M_PropUAZ" (g_propTexDir + "Prop_UAZ.png")
	append keep uaz

	local bagParts = importParts (dl + "Sand_Bag_Wall.fbx")
	local bagMat = makeShader "M_PropSandbag" (g_propTexDir + "Prop_Sandbag.png")
	local bagKinds = #(#("straight", "Prop_SandbagStraight", -16.5), #("corner", "Prop_SandbagCorner", 0.0), #("round", "Prop_SandbagRound", 0.0))
	for kind in bagKinds do
	(
		local wall = undefined
		for o in bagParts where (isValidNode o) and (findString o.name ("sand_bag_" + kind[1] + "_")) != undefined do
		(
			convertToPoly o
			if wall == undefined then wall = o else polyop.attach wall o
		)
		wall.name = kind[2]
		polyop.setFaceMatID wall #all 1
		rotate wall (eulerAngles 0 0 kind[3])
		normalizeProp wall 1.15
		local opt = ProOptimizer()
		addModifier wall opt
		opt.KeepUV = true
		select wall
		max modify mode
		modPanel.setCurrentObject opt
		opt.Calculate = true
		classOf wall
		opt.VertexPercent = 12.0
		classOf wall
		convertToPoly wall
		clearSelection()
		wall.material = bagMat
		append keep wall
	)

	for o in (objects as array) where (isValidNode o) and (findItem keep o) == 0 do delete o

	for o in keep do
	(
		local bb = nodeGetBoundingBox o (matrix3 1)
		format "% size:% min:% tris:% mat:%\n" o.name (bb[2] - bb[1]) bb[1] (getTriMeshFaceCount o)[1] (classOf o.material) to:rep
	)
	close rep
	saveMaxFile outFile quiet:true
)
