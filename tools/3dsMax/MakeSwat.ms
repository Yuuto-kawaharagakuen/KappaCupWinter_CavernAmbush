(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	frameRate = 30

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local fbxFile = (systemTools.getEnvVariable "USERPROFILE") + "\\Documents\\Swat\\SWAT+Operator.fbx"
	local texDir = projDir + "MaxData\\textures\\swat\\"
	local outFile = projDir + "MaxData\\Swat.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	global g_swatReport = createFile (projDir + "MaxData\\swat_report.txt")

	FBXImporterSetParam "Animation" true
	FBXImporterSetParam "Skin" true
	importFile fbxFile #noPrompt using:FBXIMP

	fn makeMaterial name albedo normal =
	(
		local m = DirectX_9_Shader name:name
		m.effectFile = ::g_fxFile
		m.g_albedo = openBitmap (::g_texDir + albedo)
		m.g_normal = openBitmap (::g_texDir + normal)
		m.g_useNormalMap = true
		m
	)
	global g_fxFile = fxFile
	global g_texDir = texDir

	local body = getNodeByName "sol_8_low"
	local mm = Multimaterial numsubs:4 name:"M_Swat"
	mm.materialList[1] = makeMaterial "M_SwatBody" "Swat_Body_Color.PNG" "Swat_Body_Normal.PNG"
	mm.materialList[2] = makeMaterial "M_SwatHeadGear" "Swat_HeadGear_Color.PNG" "Swat_HeadGear_Normal.PNG"
	mm.materialList[3] = makeMaterial "M_SwatEyelashes" "Swat_HeadGear_Color.PNG" "Swat_HeadGear_Normal.PNG"
	mm.materialList[4] = makeMaterial "M_SwatGun" "Swat_KSVR_Color.PNG" "Swat_KSVR_Normal.PNG"
	body.material = mm

	local gunRoot = getNodeByName "topGun_low"
	local gunParts = #(gunRoot)
	for c in gunRoot.children do append gunParts c
	local gun = copy gunRoot
	gun.parent = undefined
	convertToPoly gun
	for i = 2 to gunParts.count do
	(
		local c = copy gunParts[i]
		c.parent = undefined
		convertToPoly c
		polyop.attach gun c
	)
	polyop.setFaceMatID gun #all 4
	for p in gunParts do delete p

	local sk = body.modifiers[#Skin]
	local hand = getNodeByName "mixamorig:RightHand"
	local bindTM = skinUtils.GetBoneBindTM body hand
	local gunCheckPos = polyop.getVert gun 1
	format "hand now:%\nhand bind:%\n" hand.transform bindTM to:g_swatReport
	gun.transform = gun.transform * (inverse hand.transform) * bindTM
	local baseObj = body.baseObject
	local numBefore = polyop.getNumVerts baseObj
	select body
	max modify mode
	modPanel.setCurrentObject baseObj
	polyop.attach body gun
	body.material = mm
	local numAfter = polyop.getNumVerts body.baseObject
	format "verts before:% after:%\n" numBefore numAfter to:g_swatReport

	modPanel.setCurrentObject sk
	local handIndex = 0
	for i = 1 to (skinOps.GetNumberBones sk) do
	(
		if (skinOps.GetBoneName sk i 0) == "mixamorig:RightHand" then handIndex = i
	)
	format "hand bone index:% skin verts:%\n" handIndex (skinOps.GetNumberVertices sk) to:g_swatReport
	for v = (numBefore + 1) to numAfter do
	(
		skinOps.ReplaceVertexWeights sk v handIndex 1.0
	)
	local checkV = numBefore + 1
	format "check vert % weights:% bone:%\n" checkV (skinOps.GetVertexWeightCount sk checkV) (skinOps.GetBoneName sk (skinOps.GetVertexWeightBoneID sk checkV 1) 0) to:g_swatReport
	local deformed = snapshotAsMesh body
	format "gun vert world before:% deformed:%\n" gunCheckPos (getVert deformed checkV) to:g_swatReport
	delete deformed

	sliderTime = 0
	local low = snapshot body
	low.name = "SwatLow"
	convertToPoly low
	local opt = ProOptimizer()
	addModifier low opt
	opt.KeepUV = true
	opt.LockMat = true
	select low
	max modify mode
	modPanel.setCurrentObject opt
	opt.Calculate = true
	classOf low
	opt.VertexPercent = 18.0
	classOf low
	format "opt status:% verts:%
" opt.Calculate (getNumVerts low) to:g_swatReport
	convertToPoly low
	format "low faces:% (orig:%)\n" (polyop.getNumFaces low) (polyop.getNumFaces body.baseObject) to:g_swatReport

	local wrap = Skin_Wrap()
	addModifier low wrap
	wrap.meshList = #(body)
	wrap.engine = 0
	wrap.falloff = 1.0
	wrap.distance = 1.0
	wrap.faceLimit = 3
	wrap.weightAllVerts = true
	select low
	max modify mode
	modPanel.setCurrentObject wrap
	wrap.reset()
	wrap.ConvertToSkin false
	local newSkin = low.modifiers[#Skin]
	format "converted skin:% bones:%\n" (newSkin != undefined) (if newSkin != undefined then skinOps.GetNumberBones newSkin else 0) to:g_swatReport
	deleteModifier low wrap
	low.material = mm
	delete body
	body = low

	fn collectBones n arr =
	(
		append arr n
		for c in n.children do ::g_collectBones c arr
	)
	global g_collectBones = collectBones
	local bonesList = #()
	collectBones (getNodeByName "SWAT") bonesList
	local rest = for b in bonesList collect (at time 0 b.transform)
	local parentIndex = for b in bonesList collect (if b.parent == undefined then 0 else findItem bonesList b.parent)
	local localTM = for i = 1 to bonesList.count collect (if parentIndex[i] == 0 then rest[i] else rest[i] * (inverse rest[parentIndex[i]]))

	fn boneOffset name clip t =
	(
		local rx = 0.0
		local rz = 0.0
		local pz = 0.0
		if clip == #idle then
		(
			local ph = 360.0 * t / 60.0
			case name of
			(
				"mixamorig:Spine1": rx = 1.2 * sin ph
				"mixamorig:Neck": rx = -0.6 * sin ph
				"mixamorig:Hips": pz = 0.4 * sin ph
			)
		)
		else if clip == #walk then
		(
			local ph = 360.0 * t / 32.0
			case name of
			(
				"mixamorig:Hips": (pz = 1.5 * (cos (2.0 * ph)) - 1.5; rz = 4.0 * sin ph)
				"mixamorig:Spine": rx = 3.0
				"mixamorig:Spine1": rz = -4.0 * sin ph
				"mixamorig:LeftUpLeg": rx = -22.0 * sin ph
				"mixamorig:RightUpLeg": rx = 22.0 * sin ph
				"mixamorig:LeftLeg": rx = 6.0 + 32.0 * (amax 0.0 (cos ph))
				"mixamorig:RightLeg": rx = 6.0 + 32.0 * (amax 0.0 (-(cos ph)))
				"mixamorig:LeftFoot": rx = -8.0 * (amax 0.0 (cos ph))
				"mixamorig:RightFoot": rx = -8.0 * (amax 0.0 (-(cos ph)))
			)
		)
		else if clip == #shoot then
		(
			local kick = if t < 2 then t / 2.0 else exp (-(t - 2) / 2.5)
			case name of
			(
				"mixamorig:Spine1": rx = -5.0 * kick
				"mixamorig:Neck": rx = 2.0 * kick
				"mixamorig:RightShoulder": rx = -3.0 * kick
			)
		)
		#(rx, rz, pz)
	)
	global g_boneOffset = boneOffset

	local clips = #(#(#idle, 0, 59), #(#walk, 100, 131), #(#shoot, 200, 209))
	animationRange = interval 0 220
	with animate on
	(
		for c in clips do
		(
			for f = c[2] to c[3] do
			(
				local world = #()
				for i = 1 to bonesList.count do
				(
					local m = if parentIndex[i] == 0 then copy rest[i] else localTM[i] * world[parentIndex[i]]
					local ofs = boneOffset bonesList[i].name c[1] (f - c[2])
					if ofs[1] != 0.0 or ofs[2] != 0.0 then
					(
						local p = m.translationPart
						m = m * (transMatrix (-p)) * (rotateXMatrix ofs[1]) * (rotateZMatrix ofs[2]) * (transMatrix p)
					)
					if ofs[3] != 0.0 then m.translation = m.translation + [0, 0, ofs[3]]
					world[i] = m
					at time f bonesList[i].transform = m
				)
			)
		)
	)
	format "bones:% clips written\n" bonesList.count to:g_swatReport

	body.name = "SwatSoldier"
	close g_swatReport
	sliderTime = 0
	saveMaxFile outFile quiet:true
)
