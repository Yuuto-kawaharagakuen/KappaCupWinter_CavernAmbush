(
	local args = maxOps.mxsCmdLineArgs
	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local k2Dir = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxScripts\\sub\\"
	local maxFile = projDir + "MaxData\\" + args[#file]
	local modelDir = projDir + "Game\\Assets\\animData\\"
	local rootName = args[#root]
	local clipList = filterString args[#clips] ","
	global g_tkaOutPath = ""

	global g_exportReport = createFile (projDir + "MaxData\\export_report.txt")

	fn readAllText path =
	(
		local f = openFile path encoding:#utf8
		local txt = ""
		while not eof f do txt += (readLine f) + "\n"
		close f
		txt
	)

	units.SystemType = #Centimeters
	loadMaxFile maxFile quiet:true
	units.SystemType = #Centimeters
	makeDir modelDir all:true

	filein (k2Dir + "tkExporterCommon.ms")

	local src = readAllText (k2Dir + "tkaExporter.ms")
	src = substituteString src "getSaveFileName caption:\"\" types: \"animation clip (*tka)|*.tka|All Files (*.*)|*.*|\"" "g_tkaOutPath"
	try (execute src) catch (format "EXECUTE ERROR: %\n" (getCurrentException()) to:g_exportReport)

	local root = getNodeByName rootName
	if root == undefined then
	(
		format "MISSING ROOT: %\n" rootName to:g_exportReport
	)
	else
	(
		::g_objectParams = #()
		at time 0 (::BuildOutputNodeInfo root -1)
		select root
		for c in clipList do
		(
			local parts = filterString c ":"
			::g_tkaOutPath = modelDir + parts[1] + ".tka"
			::startFrame = parts[2] as integer
			::endFrame = parts[3] as integer
			::SaveAnimationClip()
			format "EXPORTED: % (% - %, % bones)\n" ::g_tkaOutPath ::startFrame ::endFrame ::g_objectParams.count to:g_exportReport
		)
	)

	close g_exportReport
)
