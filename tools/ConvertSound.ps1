param(
	[Parameter(Mandatory = $true)][string]$Source,
	[Parameter(Mandatory = $true)][string]$Name
)

$root = Split-Path -Parent $PSScriptRoot
$outPath = Join-Path $root "Game\Assets\sound\$Name.wav"
$workDir = Join-Path $env:TEMP "CavernAmbushSound"
New-Item -ItemType Directory -Force $workDir | Out-Null
$wavPath = (Resolve-Path $Source).Path

if ([IO.Path]::GetExtension($wavPath) -ne ".wav") {
	Add-Type -AssemblyName System.Runtime.WindowsRuntime
	$methods = [System.WindowsRuntimeSystemExtensions].GetMethods()
	$asTask = ($methods | Where-Object { $_.Name -eq "AsTask" -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq "IAsyncOperation``1" })[0]
	$asTaskProgress = ($methods | Where-Object { $_.Name -eq "AsTask" -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq "IAsyncActionWithProgress``1" })[0]
	function Await($operation, [Type]$type)
	{
		$task = $asTask.MakeGenericMethod($type).Invoke($null, @($operation))
		$task.Wait(-1) | Out-Null
		$task.Result
	}
	[Windows.Storage.StorageFile, Windows.Storage, ContentType = WindowsRuntime] | Out-Null
	[Windows.Media.Transcoding.MediaTranscoder, Windows.Media.Transcoding, ContentType = WindowsRuntime] | Out-Null
	[Windows.Media.MediaProperties.MediaEncodingProfile, Windows.Media.MediaProperties, ContentType = WindowsRuntime] | Out-Null

	$inFile = Await ([Windows.Storage.StorageFile]::GetFileFromPathAsync($wavPath)) ([Windows.Storage.StorageFile])
	$folder = Await ([Windows.Storage.StorageFolder]::GetFolderFromPathAsync($workDir)) ([Windows.Storage.StorageFolder])
	$outFile = Await ($folder.CreateFileAsync("$Name.wav", [Windows.Storage.CreationCollisionOption]::ReplaceExisting)) ([Windows.Storage.StorageFile])
	$profile = [Windows.Media.MediaProperties.MediaEncodingProfile]::CreateWav([Windows.Media.MediaProperties.AudioEncodingQuality]::High)
	$transcoder = New-Object Windows.Media.Transcoding.MediaTranscoder
	$prepared = Await ($transcoder.PrepareFileTranscodeAsync($inFile, $outFile, $profile)) ([Windows.Media.Transcoding.PrepareTranscodeResult])
	if (-not $prepared.CanTranscode) {
		throw "Cannot convert $Source : $($prepared.FailureReason)"
	}
	$task = $asTaskProgress.MakeGenericMethod([double]).Invoke($null, @($prepared.TranscodeAsync()))
	$task.Wait(-1) | Out-Null
	$wavPath = $outFile.Path
}

$bytes = [IO.File]::ReadAllBytes($wavPath)
$offset = 12
$fmt = -1
$data = -1
$dataSize = 0
while ($offset -lt $bytes.Length - 8) {
	$id = [Text.Encoding]::ASCII.GetString($bytes, $offset, 4)
	$size = [BitConverter]::ToInt32($bytes, $offset + 4)
	if ($id -eq "fmt ") { $fmt = $offset + 8 }
	elseif ($id -eq "data") { $data = $offset + 8; $dataSize = $size }
	$offset += 8 + $size + ($size % 2)
}
if ($fmt -lt 0 -or $data -lt 0) {
	throw "Not a wav file: $wavPath"
}
$tag = [BitConverter]::ToUInt16($bytes, $fmt)
$channels = [BitConverter]::ToUInt16($bytes, $fmt + 2)
$rate = [BitConverter]::ToInt32($bytes, $fmt + 4)
$bits = [BitConverter]::ToUInt16($bytes, $fmt + 14)
if (($tag -ne 1 -and $tag -ne 0xFFFE) -or $bits -lt 16) {
	throw "Unsupported wav format (tag $tag, $bits bit): $wavPath"
}
$sampleBytes = $bits / 8
$count = [int]($dataSize / $sampleBytes)
$pcm = New-Object byte[] ($count * 2)
for ($i = 0; $i -lt $count; $i++) {
	$p = $data + $i * $sampleBytes
	$pcm[$i * 2] = $bytes[$p + $sampleBytes - 2]
	$pcm[$i * 2 + 1] = $bytes[$p + $sampleBytes - 1]
}

$stream = New-Object IO.MemoryStream
$writer = New-Object IO.BinaryWriter $stream
$writer.Write([Text.Encoding]::ASCII.GetBytes("RIFF"))
$writer.Write([int](36 + $pcm.Length))
$writer.Write([Text.Encoding]::ASCII.GetBytes("WAVEfmt "))
$writer.Write([int]16)
$writer.Write([uint16]1)
$writer.Write([uint16]$channels)
$writer.Write([int]$rate)
$writer.Write([int]($rate * $channels * 2))
$writer.Write([uint16]($channels * 2))
$writer.Write([uint16]16)
$writer.Write([Text.Encoding]::ASCII.GetBytes("data"))
$writer.Write([int]$pcm.Length)
$writer.Write($pcm, 0, $pcm.Length)
$writer.Flush()
[IO.File]::WriteAllBytes($outPath, $stream.ToArray())
"{0}  {1}ch {2}Hz {3}bit -> 16bit  {4:F2}s" -f $outPath, $channels, $rate, $bits, ($count / $channels / $rate)
