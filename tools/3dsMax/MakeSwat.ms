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
	fn nodeCenter n = ( local bb = nodeGetBoundingBox n (matrix3 1); (bb[1] + bb[2]) * 0.5 )
	local gunBarrel = nodeCenter (getNodeByName "barrel_low")
	local gunStock = nodeCenter (getNodeByName "stockBack_low")
	local gunBackSight = nodeCenter (getNodeByName "backSight_low")
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

	fn rotBetween a b =
	(
		local na = normalize a
		local nb = normalize b
		local c = dot na nb
		if c > 0.99999 then return (matrix3 1)
		local ax = normalize (cross na nb)
		local ang = acos (amin 1.0 (amax -1.0 c))
		local m = (angleAxis ang ax) as matrix3
		if (dot (normalize (na * m)) nb) < 0.999 then m = (angleAxis (-ang) ax) as matrix3
		m
	)
	global g_rotBetween = rotBetween

	fn elbowPos a b c target pole =
	(
		local l1 = distance a b
		local l2 = distance b c
		local toT = target - a
		local d = amin (length toT) (l1 + l2 - 0.01)
		local dirT = normalize toT
		local cosA = (l1 * l1 + d * d - l2 * l2) / (2.0 * l1 * d)
		cosA = amin 1.0 (amax -1.0 cosA)
		local sinA = sqrt (1.0 - cosA * cosA)
		local perp = pole - dirT * (dot pole dirT)
		perp = normalize perp
		a + dirT * (cosA * l1) + perp * (sinA * l1)
	)
	global g_elbowPos = elbowPos

	local gunDir = gunBarrel - gunStock
	local yawDeg = -90.0 - (atan2 gunDir.y gunDir.x)
	local yawM = rotateZMatrix yawDeg
	if (normalize ([gunDir.x, gunDir.y, 0] * yawM)).y > -0.99 then yawM = rotateZMatrix (-yawDeg)
	local d1 = gunDir * yawM
	local pitchDeg = atan2 d1.z (-d1.y)
	local pitchM = rotateXMatrix pitchDeg
	if abs ((normalize (d1 * pitchM)).z) > 0.01 then pitchM = rotateXMatrix (-pitchDeg)
	local aimSightTarget = [-6.0, -26.0, 163.0]
	local gunX = (transMatrix (-gunBackSight)) * yawM * pitchM * (transMatrix aimSightTarget)
	format "gun dir after:% \n" (normalize (gunDir * yawM * pitchM)) to:g_swatReport
	local muzzleAim = (gunBarrel + (normalize gunDir) * 4.0) * gunX
	format "aim muzzle (max coords):%\n" muzzleAim to:g_swatReport

	fn findBone name = ( local i = 0; for j = 1 to ::g_bonesList.count do if ::g_bonesList[j].name == name then i = j; i )
	global g_bonesList = bonesList
	local spineIdx = findBone "mixamorig:Spine1"
	local armChains = #()
	for side in #(#(#("mixamorig:RightArm", "mixamorig:RightForeArm", "mixamorig:RightHand"), [-1.0, 0.0, -1.0]), #(#("mixamorig:LeftArm", "mixamorig:LeftForeArm", "mixamorig:LeftHand"), [1.0, 0.3, -1.0])) do
	(
		local idx = for n in side[1] collect findBone n
		local handLocal = rest[idx[3]] * gunX * (inverse rest[spineIdx])
		append armChains #(idx, handLocal, side[2])
	)
	global g_armChains = armChains

	fn keyInterp keys t =
	(
		if t <= keys[1][1] then return keys[1][2]
		for i = 1 to keys.count - 1 do
		(
			local k0 = keys[i]
			local k1 = keys[i + 1]
			if t <= k1[1] then
			(
				local u = (t - k0[1]) / (k1[1] - k0[1] as float)
				u = u * u * (3.0 - 2.0 * u)
				return k0[2] + (k1[2] - k0[2]) * u
			)
		)
		keys[keys.count][2]
	)
	global g_keyInterp = keyInterp

	global g_deathKeys = #(
		#("mixamorig:Hips", #pz, #(#(0, 0), #(4, 1), #(12, -6), #(24, -50), #(40, -72), #(44, -68), #(48, -72))),
		#("mixamorig:Hips", #py, #(#(0, 0), #(4, 6), #(12, 22), #(24, 28), #(40, 45))),
		#("mixamorig:Hips", #rx, #(#(0, 0), #(24, -8), #(40, -72), #(44, -70), #(48, -75))),
		#("mixamorig:Hips", #rz, #(#(0, 0), #(24, 6), #(40, 14))),
		#("mixamorig:Spine", #rx, #(#(0, 0), #(3, -8), #(14, -4), #(24, 6), #(40, -6))),
		#("mixamorig:Spine1", #rx, #(#(0, 0), #(3, -22), #(12, -12), #(24, 8), #(40, -8), #(48, -6))),
		#("mixamorig:Neck", #rx, #(#(0, 0), #(3, -18), #(12, -8), #(24, 12), #(40, -10), #(44, 4), #(48, 0))),
		#("mixamorig:Head", #rx, #(#(0, 0), #(3, -10), #(24, 6), #(40, -6))),
		#("mixamorig:LeftUpLeg", #rx, #(#(0, 0), #(4, 0), #(12, 18), #(24, -55), #(40, -20))),
		#("mixamorig:LeftLeg", #rx, #(#(0, 0), #(12, 25), #(24, 105), #(40, 70))),
		#("mixamorig:RightUpLeg", #rx, #(#(0, 0), #(12, -5), #(24, -50), #(40, -15))),
		#("mixamorig:RightLeg", #rx, #(#(0, 0), #(12, 10), #(24, 100), #(40, 60))),
		#("mixamorig:LeftFoot", #rx, #(#(0, 0), #(24, -20), #(40, 10))),
		#("mixamorig:RightFoot", #rx, #(#(0, 0), #(24, -20), #(40, 10))),
		#("mixamorig:LeftShoulder", #rx, #(#(0, 0), #(4, -10), #(24, 10), #(40, 25))),
		#("mixamorig:LeftShoulder", #rz, #(#(0, 0), #(40, -15))),
		#("mixamorig:RightShoulder", #rx, #(#(0, 0), #(4, -8), #(40, 15)))
	)

	fn boneOffset name clip t =
	(
		local rx = 0.0
		local rz = 0.0
		local pz = 0.0
		local py = 0.0
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
			)
		)
		else if clip == #aim then
		(
			local ph = 360.0 * t / 30.0
			case name of
			(
				"mixamorig:Spine1": rx = 0.6 * sin ph
				"mixamorig:LeftUpLeg": rx = -8.0
				"mixamorig:LeftLeg": rx = 10.0
				"mixamorig:RightLeg": rx = 6.0
				"mixamorig:Hips": pz = -2.0
			)
		)
		else if clip == #death then
		(
			local keys = ::g_deathKeys
			for k in keys where k[1] == name do
			(
				local v = keyInterp k[3] t
				case k[2] of
				(
					#rx: rx = v
					#rz: rz = v
					#pz: pz = v
					#py: py = v
				)
			)
		)
		else if clip == #look then
		(
			local ph = 360.0 * t / 90.0
			case name of
			(
				"mixamorig:Spine": rz = 5.0 * sin ph
				"mixamorig:Spine1": (rz = 12.0 * sin (ph - 30.0); rx = 1.0 * sin (ph * 2.0))
				"mixamorig:Neck": rz = 30.0 * sin ph
				"mixamorig:Head": rz = 18.0 * sin (ph + 20.0)
			)
		)
		#(rx, rz, pz, py)
	)
	global g_boneOffset = boneOffset

	local clips = #(#(#idle, 0, 59), #(#walk, 100, 131), #(#shoot, 200, 209), #(#death, 300, 347), #(#look, 400, 489), #(#aim, 500, 529))
	animationRange = interval 0 540
	with animate on
	(
		for c in clips do
		(
			for f = c[2] to c[3] do
			(
				local world = #()
				local isAim = (c[1] == #aim or c[1] == #shoot)
				local overrides = #()
				for i = 1 to bonesList.count do
				(
					local m = if parentIndex[i] == 0 then copy rest[i] else localTM[i] * world[parentIndex[i]]
					local ov = 0
					for k = 1 to overrides.count do if overrides[k][1] == i then ov = k
					if ov > 0 then
					(
						m = overrides[ov][2]
						if overrides[ov].count > 2 then
						(
							local cPos = (localTM[overrides[ov][3]] * m).translationPart
							local ePos = m.translationPart
							m = m * (transMatrix (-ePos)) * (rotBetween (cPos - ePos) (overrides[ov][4] - ePos)) * (transMatrix ePos)
							append overrides #(overrides[ov][3], overrides[ov][5])
						)
						world[i] = m
						at time f bonesList[i].transform = m
						continue
					)
					local ofs = boneOffset bonesList[i].name c[1] (f - c[2])
					if ofs[1] != 0.0 or ofs[2] != 0.0 then
					(
						local p = m.translationPart
						m = m * (transMatrix (-p)) * (rotateXMatrix ofs[1]) * (rotateZMatrix ofs[2]) * (transMatrix p)
					)
					if ofs[3] != 0.0 or ofs[4] != 0.0 then m.translation = m.translation + [0, ofs[4], ofs[3]]
					if isAim then
					(
						for ch in armChains where ch[1][1] == i do
						(
							local idx = ch[1]
							local handW = ch[2] * world[spineIdx]
							local target = handW.translationPart
							local aPos = m.translationPart
							local foreW0 = localTM[idx[2]] * m
							local bPos = foreW0.translationPart
							local cPos0 = (localTM[idx[3]] * foreW0).translationPart
							local ePos = elbowPos aPos bPos cPos0 target ch[3]
							m = m * (transMatrix (-aPos)) * (rotBetween (bPos - aPos) (ePos - aPos)) * (transMatrix aPos)
							append overrides #(idx[2], localTM[idx[2]] * m, idx[3], target, handW)
						)
					)
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
