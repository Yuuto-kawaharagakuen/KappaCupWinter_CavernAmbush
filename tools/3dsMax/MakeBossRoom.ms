fileIn ((getFilenamePath (getThisScriptFilename())) + "StageCommon.ms")
(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters
	seed 20261120

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	global g_texDir = projDir + "MaxData\\textures\\"
	local outFile = projDir + "MaxData\\BossRoom.max"
	global g_fxFile = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxShader\\k2EngineShader.fx"

	global G_GRID_MIN = [-2100, -1700]
	global G_GRID_MAX = [1700, 1700]
	global G_GRID_STEP = 56.0
	global G_SCALE = 1.071
	global G_MAX_HEIGHT = 1000.0
	global G_FALLOFF = 300.0
	global G_OUTSIDE_DEPTH = -250.0
	global G_NOISE_SEED = 23.9
	global G_TUNNEL_HALF_WIDTH = 95.0
	global G_TUNNEL_HEIGHT = 135.0
	global G_CLIFF_NORMAL_Z = 0.55
	global G_PLATEAU_HEADROOM = 380.0

	global g_caveEllipses = #(
		#([0, 0], [1250, 1150])
	)
	global g_caveCapsules = #(
		#([-1850, 0], [-1100, 0], 230)
	)
	global g_diveTunnels = #()
	global g_plateaus = #()
	local playerStartRaw = [-1550, 0]
	local bossStartRaw = [450, 0]
	local doorRaw = [-1830, 0]

	local matSand = makeStageMat "M_CaveSand" "Ground079S_2K-JPG_Color.jpg" "Ground079S_2K-JPG_NormalDX.jpg"
	local matWall = makeStageMat "M_CaveWall" "Rock061_2K-JPG_Color.jpg" "Rock061_2K-JPG_NormalDX.jpg"
	local matCeil = makeStageMat "M_CaveCeiling" "Rock060_2K-JPG_Color.jpg" "Rock060_2K-JPG_NormalDX.jpg"
	local matWood = makeStageMat "M_Wood" "T_Wood.png" ""

	local floorObj = buildFloor "Floor_BossRoom" matSand matWall
	local pillars = #(#([-450, 480], 120), #([-450, -480], 120), #([500, 620], 110), #([500, -620], 110))
	local mounds = #(#([-750, 0], 110, 150), #([100, 300], 90, 130), #([100, -300], 90, 130), #([850, 0], 100, 140))
	local shellObj = buildShell "Wall_BossRoom" matWall matCeil pillars 12 mounds
	local doorObj = makeDoor "Gate_BossRoom" doorRaw 500 matWood

	local root = Dummy name:"BossRoom" pos:[0,0,0] boxsize:[200,200,200]
	for o in #(floorObj, shellObj, doorObj) do o.parent = root
	addStagePoint "PlayerStart" playerStartRaw 60 root
	addStagePoint "BossStart" bossStartRaw 0 root

	local lightData = #(
		#("Light_Lamp", [-1450, 0], 220),
		#("Light_LampRed", [-700, 600], 320),
		#("Light_LampRed", [-700, -600], 320),
		#("Light_LampRed", [300, 850], 320),
		#("Light_LampRed", [300, -850], 320),
		#("Light_LampRed", [950, 0], 320),
		#("Light_Lamp", [0, 0], 450)
	)
	addStageLights lightData root

	local rep = createFile (projDir + "MaxData\\bossroom_report.txt")
	writeStageMap rep 100.0
	writeClearance rep "start" playerStartRaw
	writeClearance rep "boss" bossStartRaw
	writeClearance rep "center" [0, 0]
	writeClearance rep "edge" [900, 600]
	for d in lightData do writeClearance rep d[1] d[2]
	format "Floor faces: % Wall faces: %\n" (polyop.getNumFaces floorObj) (polyop.getNumFaces shellObj) to:rep
	saveMaxFile outFile quiet:true
	format "Saved: %\n" outFile to:rep
	close rep
)
