fileIn ((getFilenamePath (getThisScriptFilename())) + "StageCommon.ms")
(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	seed 20261119

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	global g_texDir = projDir + "MaxData\\textures\\"
	local outFile = projDir + "MaxData\\Stage2.max"
	global g_fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	global G_GRID_MIN = [-3900, -2900]
	global G_GRID_MAX = [4300, 2900]
	global G_GRID_STEP = 56.0
	global G_SCALE = 1.071
	global G_MAX_HEIGHT = 700.0
	global G_FALLOFF = 450.0
	global G_OUTSIDE_DEPTH = -250.0
	global G_NOISE_SEED = 11.3
	global G_TUNNEL_HALF_WIDTH = 95.0
	global G_TUNNEL_HEIGHT = 135.0
	global G_CLIFF_NORMAL_Z = 0.55
	global G_PLATEAU_HEADROOM = 380.0

	global g_caveEllipses = #(
		#([0, 0], [1500, 1100]),
		#([-2900, -300], [600, 480]),
		#([-1500, 2000], [750, 550]),
		#([200, -2050], [850, 550]),
		#([1700, 1850], [800, 620]),
		#([2900, 0], [750, 650])
	)
	global g_caveCapsules = #(
		#([-2400, -250], [-1350, -150], 260),
		#([-1050, 750], [-1350, 1550], 260),
		#([150, -1000], [200, -1600], 280),
		#([950, 800], [1350, 1350], 260),
		#([1350, 0], [2250, 0], 280),
		#([3500, 0], [4000, 0], 200)
	)
	global g_diveTunnels = #(
		#([-3350, -300], [-3800, -300]),
		#([-850, 2250], [0, 2500], [950, 2350]),
		#([1000, -2150], [2000, -1550], [2650, -700])
	)
	global g_plateaus = #()
	local playerStartRaw = [-3650, -300]
	local stageBackRaw = [-3800, -300]
	local bossDoorRaw = [3880, 0]
	local doorRaw = [4030, 0]

	local matSand = makeStageMat "M_CaveSand" "Ground079S_2K-JPG_Color.jpg" "Ground079S_2K-JPG_NormalDX.jpg"
	local matWall = makeStageMat "M_CaveWall" "Rock061_2K-JPG_Color.jpg" "Rock061_2K-JPG_NormalDX.jpg"
	local matCeil = makeStageMat "M_CaveCeiling" "Rock060_2K-JPG_Color.jpg" "Rock060_2K-JPG_NormalDX.jpg"
	local matWood = makeStageMat "M_Wood" "T_Wood.png" ""

	local floorObj = buildFloor "Floor_Stage2" matSand matWall
	local pillars = #(#([-600, 450], 150), #([350, 600], 130))
	local mounds = #(#([-900, -600], 100, 180), #([-1700, 2250], 90, 160), #([500, -2300], 110, 200), #([3200, 350], 90, 170), #([2400, 1900], 80, 150))
	local shellObj = buildShell "Wall_Stage2" matWall matCeil pillars 30 mounds
	local doorObj = makeDoor "Gate_Stage2" doorRaw 460 matWood

	local root = Dummy name:"Stage2" pos:[0,0,0] boxsize:[200,200,200]
	for o in #(floorObj, shellObj, doorObj) do o.parent = root
	addStagePoint "PlayerStartDive" playerStartRaw 5 root
	addStagePoint "StageBack" stageBackRaw 0 root
	addStagePoint "BossDoor" bossDoorRaw 0 root

	local lightData = #(
		#("Light_Lamp", [-2800, -100], 220),
		#("Light_Lamp", [-1900, -200], 220),
		#("Light_LampRed", [-1000, -100], 260),
		#("Light_LampRed", [0, 300], 300),
		#("Light_LampRed", [700, -150], 280),
		#("Light_Lamp", [-1200, 1150], 220),
		#("Light_LampRed", [-1500, 1950], 240),
		#("Light_Lamp", [180, -1300], 220),
		#("Light_LampRed", [300, -2000], 240),
		#("Light_Lamp", [1150, 1100], 220),
		#("Light_LampRed", [1500, 1700], 260),
		#("Light_Lamp", [1800, 0], 220),
		#("Light_LampRed", [2850, -150], 260),
		#("Light_Lamp", [3650, 0], 200)
	)
	addStageLights lightData root

	local rep = createFile (projDir + "MaxData\\stage2_report.txt")
	writeStageMap rep 100.0
	writeClearance rep "start" playerStartRaw
	writeClearance rep "bossDoor" bossDoorRaw
	for d in lightData do writeClearance rep d[1] d[2]
	local minNormalZ = 1.0
	for f = 1 to (polyop.getNumFaces floorObj) do minNormalZ = amin minNormalZ (polyop.getFaceNormal floorObj f).z
	format "Floor steepest: % deg\n" (acos minNormalZ) to:rep
	format "Floor faces: % Wall faces: %\n" (polyop.getNumFaces floorObj) (polyop.getNumFaces shellObj) to:rep
	saveMaxFile outFile quiet:true
	format "Saved: %\n" outFile to:rep
	close rep
)
