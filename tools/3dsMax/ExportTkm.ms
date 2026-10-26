(
	local args = maxOps.mxsCmdLineArgs
	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local k2Dir = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxScripts\\sub\\"
	local maxFile = projDir + "MaxData\\" + args[#file]
	local modelDir = projDir + "Game\\Assets\\modelData\\"
	local exportNames = filterString args[#names] ","

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

	filein (k2Dir + "tkExporterCommon.ms")
	filein (k2Dir + "tksExporter.ms")

	local src = readAllText (k2Dir + "tkmExporter.ms")
	src = substituteString src "filePath = getSaveFileName caption:\"\" types: \" (*.tkm)|*.tkm|All Files (*.*)|*.*|\"" ""
	src = substituteString src "ShellLaunch command arg" "()"
	src = substituteString src "messageBox errorMessage" "format \"EXPORT ERROR: %\\n\" errorMessage to:g_exportReport"
	try (execute src) catch (format "EXECUTE ERROR: %\n" (getCurrentException()) to:g_exportReport)
	local exporterDef = globalVars.get #TkmExporterForScript

	for nm in exportNames do
	(
		local n = getNodeByName nm
		if n == undefined then
		(
			format "MISSING NODE: %\n" nm to:g_exportReport
		)
		else
		(
			local outPath = modelDir + nm + ".tkm"
			local exporter = exporterDef()
			exporter.SaveTkm #(n) outPath false
			format "EXPORTED: %\n" outPath to:g_exportReport
		)
	)

	close g_exportReport
)
