(
	local args = maxOps.mxsCmdLineArgs
	local scriptDir = getFilenamePath (getThisScriptFilename())
	local projDir = pathConfig.normalizePath (scriptDir + "..\\..\\")
	local k2Dir = "C:\\my\\02_Git\\01_k2Engine\\GameTemplate\\tools\\3dsMaxScripts\\sub\\"
	local maxFile = projDir + "MaxData\\" + args[#file]
	local levelName = args[#level]
	global g_tklOutPath = projDir + "Game\\Assets\\level3D\\" + levelName + ".tkl"

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

	local src = readAllText (k2Dir + "tklExporter.ms")
	src = substituteString src "getSaveFileName caption:\"\" types: \"level (*.tkl)|*.tkl|All Files (*.*)|*.*|\"" "g_tklOutPath"
	src = substituteString src "Messagebox \"" "format \"EXPORT ERROR \" to:g_exportReport; format \""
	try (execute src) catch (format "EXECUTE ERROR: %\n" (getCurrentException()) to:g_exportReport)

	local root = getNodeByName levelName
	if root == undefined then
	(
		format "MISSING LEVEL ROOT: %\n" levelName to:g_exportReport
	)
	else
	(
		::g_objectParams = #()
		at time 0
		(
			::BuildOutputNodeInfo root -1
			::SaveLevel()
		)
		format "EXPORTED: % (% objects)\n" g_tklOutPath ::g_objectParams.count to:g_exportReport
	)

	close g_exportReport
)
