(
	resetMaxFile #noPrompt
	units.SystemType = #Centimeters
	units.DisplayType = #Metric
	units.MetricType = #Centimeters

	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local fbxFile = (systemTools.getEnvVariable "USERPROFILE") + "\\Downloads\\Tactical_Soldier_02.Fbx(1).fbx"
	local outDir = projDir + "MaxData\\Mixamo\\"
	makeDir outDir all:true
	local outFile = outDir + "Boss_ForMixamo.fbx"
	local report = createFile (outDir + "boss_mixamo_report.txt")

	FBXImporterSetParam "Animation" false
	FBXImporterSetParam "Skin" true
	importFile fbxFile #noPrompt using:FBXIMP

	local keepNames = #("CC_Base_Body", "CC_Base_Eye", "CC_Base_Teeth", "CC_Base_Tongue", "Glass_LP1", "Shirt_02_LP2", "Pants_LP2")
	delete (for o in geometry where findItem keepNames o.name == 0 collect o)
	delete (for o in objects where superClassOf o != GeometryClass collect o)

	local total = 0
	for o in geometry do
	(
		for i = o.modifiers.count to 1 by -1 do deleteModifier o i
		convertToMesh o
		resetXForm o
		convertToMesh o
		local nv = getNumVerts o
		total += nv
		format "% verts:% min:% max:%\n" o.name nv o.min o.max to:report
	)
	format "total verts:%\n" total to:report

	FBXExporterSetParam "Animation" false
	FBXExporterSetParam "Skin" false
	FBXExporterSetParam "Shape" false
	FBXExporterSetParam "EmbedTextures" false
	FBXExporterSetParam "UpAxis" "Y"
	FBXExporterSetParam "FileVersion" "FBX201800"
	exportFile outFile #noPrompt using:FBXEXP
	format "exported:% size:%\n" outFile (getFileSize outFile) to:report
	close report
)
