Add-Type -AssemblyName System.Drawing

$root = Split-Path -Parent $PSScriptRoot
$srcDir = Join-Path $root "MaxData\textures\ammo"
$outDir = Join-Path $root "Game\Assets\modelData"
$texconv = "C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\texconv.exe"
New-Item -ItemType Directory -Force $srcDir | Out-Null

function Save-MetalTexture($path, $r, $g, $b)
{
	$size = 64
	$bmp = New-Object System.Drawing.Bitmap $size, $size
	for ($y = 0; $y -lt $size; $y++) {
		for ($x = 0; $x -lt $size; $x++) {
			$t = 0.85 + 0.15 * [Math]::Sin($x / $size * [Math]::PI * 2.0)
			$c = [System.Drawing.Color]::FromArgb([int]($r * $t), [int]($g * $t), [int]($b * $t))
			$bmp.SetPixel($x, $y, $c)
		}
	}
	$bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
	$bmp.Dispose()
}

$brass = Join-Path $srcDir "Ammo_Brass.png"
$copper = Join-Path $srcDir "Ammo_Copper.png"
Save-MetalTexture $brass 213 158 40
Save-MetalTexture $copper 172 86 32

foreach ($path in @($brass, $copper)) {
	& $texconv -vflip -f BC7_UNORM -srgb -y -o $outDir $path | Out-Null
}
