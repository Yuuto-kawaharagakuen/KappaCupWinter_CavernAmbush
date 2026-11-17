(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	frameRate = 30

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local dl = (systemTools.getEnvVariable "USERPROFILE") + "\\Downloads\\"
	local texDir = projDir + "MaxData\\textures\\boss\\"
	local outFile = projDir + "MaxData\\Boss.max"
	local fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"
	global g_bossReport = createFile (projDir + "MaxData\\boss_report.txt")

	local clipDefs = #(#("Rifle Aiming Idle", 0), #("Firing Rifle", 100), #("Toss Grenade", 200), #("Falling Back Death", 300), #("Hit Reaction", 500))

	fn collectBones n arr =
	(
		append arr n
		for c in n.children do ::g_collectBones c arr
	)
	global g_collectBones = collectBones

	fn lastKeyTime bonesList =
	(
		local hi = 0
		for o in bonesList do
		(
			for c in #(o.position.controller, o.rotation.controller) do
			(
				local ks = try c.keys catch #()
				if ks.count > 0 and (ks[ks.count].time as integer) / TicksPerFrame > hi then hi = (ks[ks.count].time as integer) / TicksPerFrame
				for i = 1 to (try c.numSubs catch 0) do
				(
					local sk = try c[i].controller.keys catch #()
					if sk.count > 0 and (sk[sk.count].time as integer) / TicksPerFrame > hi then hi = (sk[sk.count].time as integer) / TicksPerFrame
				)
			)
		)
		hi
	)

	fn importFbx path =
	(
		FBXImporterSetParam "Mode" #create
		FBXImporterSetParam "Animation" true
		FBXImporterSetParam "Skin" true
		importFile path #noPrompt using:FBXIMP
	)

	local clipData = #()
	for c in clipDefs do
	(
		resetMaxFile #noPrompt
		units.SystemType = #Centimeters
		frameRate = 30
		importFbx (dl + c[1] + ".fbx")
		local hips = getNodeByName "mixamorig:Hips"
		local bl = #()
		collectBones hips bl
		local last = lastKeyTime bl
		local frames = #()
		for f = 0 to last do
		(
			at time f
			(
				append frames (for b in bl collect (if b == hips then b.transform else b.transform * (inverse b.parent.transform)))
			)
		)
		append clipData #(c[2], last, (for b in bl collect b.name), frames)
		format "clip % offset:% frames:0..% bones:%\n" c[1] c[2] last bl.count to:g_bossReport
	)

	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	frameRate = 30
	importFbx (dl + "Rifle Aiming Idle.fbx")
	sliderTime = 0
	local srcNames = #("CC_Base_Body", "CC_Base_Eye", "Glass_LP1", "Shirt_02_LP2", "Pants_LP2")
	local srcNodes = for n in srcNames collect getNodeByName n
	delete (for n in #("CC_Base_Tongue", "CC_Base_Teeth") where getNodeByName n != undefined collect getNodeByName n)
	local bodyNode = getNodeByName "CC_Base_Body"

	fn makeMat matName albedo normal =
	(
		local m = DirectX_9_Shader name:matName
		m.effectFile = ::g_bossFx
		m.g_albedo = openBitmap (::g_bossTexDir + albedo)
		if normal != "" then
		(
			m.g_normal = openBitmap (::g_bossTexDir + normal)
			m.g_useNormalMap = true
		)
		else m.g_useNormalMap = false
		m.g_useMetallicSmoothMap = false
		m
	)
	global g_bossFx = fxFile
	global g_bossTexDir = texDir

	local matDefs = #(
		#("M_BossSkinHead", "Boss_Std_Skin_Head_Diffuse.jpg", "Boss_Std_Skin_Head_Normal.png"),
		#("M_BossSkinBody", "Boss_Std_Skin_Body_Diffuse.jpg", "Boss_Std_Skin_Body_Normal.png"),
		#("M_BossSkinArm", "Boss_Std_Skin_Arm_Diffuse.jpg", "Boss_Std_Skin_Arm_Normal.png"),
		#("M_BossSkinLeg", "Boss_Std_Skin_Leg_Diffuse.jpg", "Boss_Std_Skin_Leg_Normal.png"),
		#("M_BossNails", "Boss_Std_Nails_Diffuse.jpg", "Boss_Std_Nails_Normal.png"),
		#("M_BossEyeR", "Boss_Std_Eye_R_Diffuse.jpg", ""),
		#("M_BossEyeL", "Boss_Std_Eye_L_Diffuse.jpg", ""),
		#("M_BossGlass", "Boss_Glass_LP2_Diffuse.png", "Boss_Glass_LP2_Normal.png"),
		#("M_BossShirt", "Boss_Shirt_02_Diffuse.png", "Boss_Shirt_02_Normal.png"),
		#("M_BossPants", "Boss_Pants_LP3_Diffuse.png", "Boss_Pants_LP3_Normal.png"),
		#("M_BossGearOlive", "Boss_GearOlive.png", ""),
		#("M_BossGearBlack", "Boss_GearBlack.png", ""),
		#("M_BossGrenade", "Boss_Grenade.png", ""),
		#("M_BossJetpack", "Boss_Jetpack.png", ""),
		#("M_BossGunBody", "Boss_m4_AR15_BaseColor.png", "Boss_m4_AR15_Normal.png"),
		#("M_BossGunMagazine", "Boss_m4_Magazine_BaseColor.png", "Boss_m4_Magazine_Normal.png"),
		#("M_BossGunScope", "Boss_m4_acog_BaseColor.png", "Boss_m4_acog_Normal.png"),
		#("M_BossGunRail", "Boss_m4_rail_BaseColor.png", "Boss_m4_rail_Normal.png"),
		#("M_BossGunSuppressor", "Boss_m4_suppresor_BaseColor.png", "Boss_m4_suppresor_Normal.png"),
		#("M_BossGunGrip", "Boss_m4_Front_Grip_BaseColor.png", "Boss_m4_Front_Grip_Normal.png"),
		#("M_BossGunLaser", "Boss_m4_laser_BaseColor.png", "Boss_m4_laser_Normal.png")
	)
	local mm = Multimaterial numsubs:matDefs.count name:"M_Boss"
	for i = 1 to matDefs.count do mm.materialList[i] = makeMat matDefs[i][1] matDefs[i][2] matDefs[i][3]

	fn snapPoly srcNode idMap =
	(
		local n = Editable_Mesh name:(srcNode.name + "_snap")
		n.mesh = snapshotAsMesh srcNode
		convertToPoly n
		local del = #{}
		for f = 1 to (polyop.getNumFaces n) do
		(
			local newId = idMap[polyop.getFaceMatID n f]
			if newId == 0 then del[f] = true else polyop.setFaceMatID n f newId
		)
		if not del.isEmpty then polyop.deleteFaces n del delIsoVerts:true
		n
	)

	local combined = snapPoly bodyNode #(1, 2, 3, 4, 5, 0)
	combined.name = "Boss"
	local parts = #(
		#("CC_Base_Eye", #(6, 0, 7, 0)),
		#("Glass_LP1", #(8)),
		#("Shirt_02_LP2", #(9)),
		#("Pants_LP2", #(10))
	)
	for p in parts do polyop.attach combined (snapPoly (getNodeByName p[1]) p[2])
	local bodyVertCount = polyop.getNumVerts combined
	format "body verts:%\n" bodyVertCount to:g_bossReport

	fn bindTM boneName = skinUtils.GetBoneBindTM ::g_bossBodyNode (getNodeByName boneName)
	global g_bossBodyNode = bodyNode
	select bodyNode
	max modify mode
	modPanel.setCurrentObject bodyNode.modifiers[#Skin]

	local gearPieces = #()
	fn addGear obj boneName matId =
	(
		try (obj.mapCoords = true) catch ()
		convertToPoly obj
		polyop.setFaceMatID obj #all matId
		local b = getNodeByName boneName
		local bt = skinUtils.GetBoneBindTM ::g_bossBodyNode b
		obj.transform = obj.transform * (inverse bt) * b.transform
		append ::g_bossGear #(obj, boneName)
		obj
	)
	global g_bossGear = gearPieces

	local helmet = Sphere radius:13.0 segs:28 hemisphere:0.5 pos:[0, 1.8, 171.8]
	helmet.scale = [0.95, 1.05, 1.0]
	addGear helmet "mixamorig:Head" 12
	local rim = Torus radius1:12.6 radius2:0.9 segs:28 sides:8 pos:[0, 1.8, 172.2]
	rim.scale = [0.95, 1.05, 1.0]
	addGear rim "mixamorig:Head" 12
	local goggleBand = Box length:3.5 width:16.0 height:4.5 pos:[0, -11.6, 176.5]
	addGear goggleBand "mixamorig:Head" 12
	for x in #(-4.0, 4.0) do
	(
		local lens = Cylinder radius:2.4 height:1.6 sides:16 pos:[x, -13.0, 178.75]
		rotate lens (angleAxis 90 [1, 0, 0])
		addGear lens "mixamorig:Head" 12
	)
	for x in #(-1.0, 1.0) do
	(
		local nv = Cylinder radius:1.6 height:7.0 sides:10 pos:[x * 3.6, -12.4, 180.5]
		rotate nv (angleAxis 90 [1, 0, 0])
		addGear nv "mixamorig:Head" 12
	)

	local frontPlate = Box length:4.0 width:30.0 height:26.0 pos:[0, -18.5, 120.0]
	addGear frontPlate "mixamorig:Spine2" 11
	local backPlate = Box length:4.0 width:30.0 height:26.0 pos:[0, 20.5, 122.0]
	addGear backPlate "mixamorig:Spine2" 11
	for x in #(-9.5, 0.0, 9.5) do
	(
		local pouch = Box length:5.0 width:8.0 height:11.0 pos:[x, -23.0, 121.0]
		addGear pouch "mixamorig:Spine2" 11
		local flap = Box length:5.6 width:8.4 height:2.4 pos:[x, -23.0, 131.0]
		addGear flap "mixamorig:Spine2" 12
		local mag = Box length:2.6 width:6.0 height:3.0 pos:[x, -22.0, 133.2]
		addGear mag "mixamorig:Spine2" 12
	)
	for x in #(-11.0, 11.0) do
	(
		local strap = Box length:3.0 width:5.5 height:24.0 pos:[x, 1.0, 146.0]
		rotate strap (angleAxis 90 [1, 0, 0])
		addGear strap "mixamorig:Spine2" 11
	)

	local belt = Tube radius1:18.6 radius2:16.8 height:4.5 sides:32 pos:[0, 0.6, 97.5]
	belt.scale = [1.0, 0.8, 1.0]
	addGear belt "mixamorig:Hips" 12
	for a in #(-150.0, -125.0, -55.0, -30.0, 30.0, 150.0) do
	(
		local px = 20.6 * cos a
		local py = 0.6 + 16.6 * sin a
		local g = Sphere radius:3.3 segs:14 pos:[px, py, 99.8]
		g.scale = [1.0, 1.0, 1.25]
		addGear g "mixamorig:Hips" 13
		local fuse = Cylinder radius:1.1 height:2.6 sides:8 pos:[px, py, 103.6]
		addGear fuse "mixamorig:Hips" 12
		local lever = Box length:1.0 width:1.2 height:5.0 pos:[px * 1.12, py * 1.12, 100.5]
		addGear lever "mixamorig:Hips" 12
	)

	local pack = Box length:12.0 width:26.0 height:32.0 pos:[0, 29.0, 120.0]
	addGear pack "mixamorig:Spine2" 14
	local nozzleExits = #()
	for x in #(-9.5, 9.5) do
	(
		local tank = Cylinder radius:5.6 height:34.0 sides:18 pos:[x, 38.0, 117.0]
		addGear tank "mixamorig:Spine2" 14
		local cap = Sphere radius:5.6 segs:16 hemisphere:0.5 pos:[x, 38.0, 151.0]
		addGear cap "mixamorig:Spine2" 14
		local nozzle = Cone radius1:5.2 radius2:3.6 height:9.0 sides:18 pos:[x, 38.0, 108.0]
		addGear nozzle "mixamorig:Spine2" 12
		append nozzleExits [x, 38.0, 108.0]
	)
	local fin = Box length:3.0 width:22.0 height:4.0 pos:[0, 36.0, 145.0]
	addGear fin "mixamorig:Spine2" 12

	local gearRanges = #()
	for g in gearPieces do
	(
		local first = (polyop.getNumVerts combined) + 1
		polyop.attach combined g[1]
		append gearRanges #(first, polyop.getNumVerts combined, g[2])
	)
	format "after gear verts:%\n" (polyop.getNumVerts combined) to:g_bossReport

	local before = for o in objects collect o
	FBXImporterSetParam "Mode" #merge
	FBXImporterSetParam "Animation" false
	importFile (dl + "m4-carabine\\source\\m4.fbx") #noPrompt using:FBXIMP
	local gunNodes = for o in objects where findItem before o == 0 and superClassOf o == GeometryClass collect o
	local gunMatIds = #(#("AR15", 15), #("Magazine", 16), #("acog", 17), #("rail", 18), #("suppresor", 19), #("Front Grip", 20), #("laser", 21))
	local gun = undefined
	for g in gunNodes do
	(
		if findItem #("glow stick", "tape", "kill flash") g.name > 0 then
		(
			delete g
		)
		else
		(
			convertToPoly g
			local id = 15
			for m in gunMatIds where g.material != undefined and g.material.name == m[1] do id = m[2]
			polyop.setFaceMatID g #all id
			if gun == undefined then
			(
				gun = g
				gun.name = "BossGun"
			)
			else polyop.attach gun g
		)
	)
	delete (for o in objects where findItem before o == 0 and o != gun and isValidNode o collect o)
	resetXForm gun
	convertToPoly gun
	format "gun verts raw:%\n" (polyop.getNumVerts gun) to:g_bossReport

	local opt = ProOptimizer()
	addModifier gun opt
	opt.KeepUV = true
	opt.LockMat = true
	select gun
	max modify mode
	modPanel.setCurrentObject opt
	opt.Calculate = true
	classOf gun
	opt.VertexPercent = 22.0
	classOf gun
	convertToPoly gun
	format "gun verts reduced:%\n" (polyop.getNumVerts gun) to:g_bossReport

	local gunScale = 0.115
	local gripLocal = [0.0, -50.0, -85.0]
	local muzzleLocal = [0.0, 463.3, 0.0]
	local rh = getNodeByName "mixamorig:RightHand"
	local rhi = getNodeByName "mixamorig:RightHandIndex1"
	local lh = getNodeByName "mixamorig:LeftHand"
	local lhi = getNodeByName "mixamorig:LeftHandIndex1"
	local palm = rh.transform.pos + 0.45 * (rhi.transform.pos - rh.transform.pos)
	local leftPalm = lh.transform.pos + 0.6 * (lhi.transform.pos - lh.transform.pos)
	local fwd = normalize (leftPalm - palm)
	local side = normalize (cross fwd [0, 0, 1])
	local up = normalize (cross side fwd)
	local rotTM = matrix3 side fwd up [0, 0, 0]
	local gunTM = (scaleMatrix [gunScale, gunScale, gunScale]) * rotTM
	gunTM.row4 = palm - (gripLocal * gunTM)
	gun.transform = gunTM
	resetXForm gun
	convertToPoly gun
	format "palm:% fwd:% up:%\n" palm fwd up to:g_bossReport

	local gunFirst = (polyop.getNumVerts combined) + 1
	polyop.attach combined gun
	append gearRanges #(gunFirst, polyop.getNumVerts combined, "mixamorig:RightHand")
	format "total verts:%\n" (polyop.getNumVerts combined) to:g_bossReport

	combined.material = mm
	local wrap = Skin_Wrap()
	addModifier combined wrap
	wrap.meshList = srcNodes
	wrap.engine = 0
	wrap.falloff = 1.0
	wrap.distance = 1.0
	wrap.faceLimit = 3
	wrap.weightAllVerts = true
	select combined
	max modify mode
	modPanel.setCurrentObject wrap
	wrap.reset()
	wrap.ConvertToSkin false
	local sk = combined.modifiers[#Skin]
	deleteModifier combined wrap
	format "skin:% bones:%\n" (sk != undefined) (if sk != undefined then skinOps.GetNumberBones sk else 0) to:g_bossReport

	modPanel.setCurrentObject sk
	local boneIndex = #()
	for i = 1 to (skinOps.GetNumberBones sk) do append boneIndex (skinOps.GetBoneName sk i 0)
	for r in gearRanges do
	(
		local bi = findItem boneIndex r[3]
		if bi == 0 then
		(
			skinOps.addBone sk (getNodeByName r[3]) 1
			append boneIndex r[3]
			bi = boneIndex.count
			format "added bone %\n" r[3] to:g_bossReport
		)
		for v = r[1] to r[2] do skinOps.ReplaceVertexWeights sk v bi 1.0
	)

	local spine2 = getNodeByName "mixamorig:Spine2"
	local s2bind = skinUtils.GetBoneBindTM bodyNode spine2
	local jetWorld = for p in nozzleExits collect (p * (inverse s2bind) * spine2.transform)
	delete srcNodes

	local root = Dummy name:"BossRoot" pos:[0, 0, 0] boxsize:[10, 10, 10]
	local hips = getNodeByName "mixamorig:Hips"
	hips.parent = root

	fn makeHelper helperName parentName worldPos =
	(
		local p = Point name:helperName size:5 pos:worldPos
		p.parent = getNodeByName parentName
		p
	)
	makeHelper "BossMuzzle" "mixamorig:RightHand" (muzzleLocal * gunTM)
	makeHelper "BossJetL" "mixamorig:Spine2" jetWorld[2]
	makeHelper "BossJetR" "mixamorig:Spine2" jetWorld[1]
	format "muzzle:% jetL:% jetR:%\n" (getNodeByName "BossMuzzle").pos (getNodeByName "BossJetL").pos (getNodeByName "BossJetR").pos to:g_bossReport

	local bonesList = #()
	collectBones hips bonesList
	bonesList = for b in bonesList where matchPattern b.name pattern:"mixamorig:*" collect b
	for b in bonesList do deleteKeys b #allKeys
	animate on
	(
		for c in clipData do
		(
			local names = c[3]
			local nodes = for n in names collect getNodeByName n
			for f = 0 to c[2] do
			(
				local tms = c[4][f + 1]
				at time (c[1] + f)
				(
					for i = 1 to nodes.count where nodes[i] != undefined do
					(
						if i == 1 then nodes[i].transform = tms[i]
						else nodes[i].transform = tms[i] * nodes[i].parent.transform
					)
				)
			)
		)
	)
	animationRange = interval 0 (clipData[clipData.count][1] + clipData[clipData.count][2])
	sliderTime = 0

	format "clips: " to:g_bossReport
	for i = 1 to clipData.count do format "%:%..% " clipDefs[i][1] clipData[i][1] (clipData[i][1] + clipData[i][2]) to:g_bossReport
	format "\n" to:g_bossReport
	saveMaxFile outFile quiet:true
	format "saved %\n" outFile to:g_bossReport
	close g_bossReport
)
