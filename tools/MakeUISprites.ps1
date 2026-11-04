Add-Type -AssemblyName System.Drawing

$root = Split-Path -Parent $PSScriptRoot
$srcDir = Join-Path $root "MaxData\textures\ui"
$outDir = Join-Path $root "Game\Assets\sprite"
$texconv = "C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\texconv.exe"
New-Item -ItemType Directory -Force $srcDir | Out-Null

function New-Canvas([int]$w, [int]$h) {
	$bmp = New-Object System.Drawing.Bitmap $w, $h, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
	$g = [System.Drawing.Graphics]::FromImage($bmp)
	$g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
	$g.Clear([System.Drawing.Color]::Transparent)
	return @($bmp, $g)
}

function Save-Png($bmp, $g, [string]$name) {
	$g.Dispose()
	$path = Join-Path $srcDir "$name.png"
	$bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
	$bmp.Dispose()
	& $texconv -f BC7_UNORM -srgb -y -o $outDir $path | Out-Null
}

function New-RoundPath([float]$x, [float]$y, [float]$w, [float]$h, [float]$r) {
	$path = New-Object System.Drawing.Drawing2D.GraphicsPath
	$d = $r * 2
	$path.AddArc($x, $y, $d, $d, 180, 90)
	$path.AddArc($x + $w - $d, $y, $d, $d, 270, 90)
	$path.AddArc($x + $w - $d, $y + $h - $d, $d, $d, 0, 90)
	$path.AddArc($x, $y + $h - $d, $d, $d, 90, 90)
	$path.CloseFigure()
	return $path
}

function New-OutlinedRound([string]$name, [int]$w, [int]$h, [float]$r, [int]$border) {
	$bmp, $g = New-Canvas $w $h
	$outer = New-RoundPath 0 0 ($w - 1) ($h - 1) $r
	$g.FillPath((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(150, 0, 0, 0))), $outer)
	if ($border -gt 0) {
		$inner = New-RoundPath $border $border ($w - 1 - $border * 2) ($h - 1 - $border * 2) ([math]::Max(1, $r - $border))
	}
	else {
		$inner = $outer
	}
	$g.FillPath([System.Drawing.Brushes]::White, $inner)
	Save-Png $bmp $g $name
}

function New-RadialShade([string]$name, [int]$w, [int]$h, [int]$alpha) {
	$bmp, $g = New-Canvas $w $h
	$path = New-Object System.Drawing.Drawing2D.GraphicsPath
	$path.AddEllipse(0, 0, $w, $h)
	$brush = New-Object System.Drawing.Drawing2D.PathGradientBrush $path
	$brush.CenterColor = [System.Drawing.Color]::FromArgb($alpha, 0, 0, 0)
	$brush.SurroundColors = @([System.Drawing.Color]::FromArgb(0, 0, 0, 0))
	$g.FillPath($brush, $path)
	Save-Png $bmp $g $name
}

$bmp, $g = New-Canvas 16 16
$g.Clear([System.Drawing.Color]::White)
Save-Png $bmp $g "UI_White"

$bmp, $g = New-Canvas 340 12
$g.FillPath([System.Drawing.Brushes]::White, (New-RoundPath 0 0 339 11 5.5))
Save-Png $bmp $g "UI_Bar"

New-OutlinedRound "UI_Pip" 16 32 4 2
New-OutlinedRound "UI_CrossLine" 8 20 3 1
New-OutlinedRound "UI_HitLine" 8 24 3 1

$bmp, $g = New-Canvas 12 12
$g.FillEllipse((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(150, 0, 0, 0))), 1, 1, 9, 9)
$g.FillEllipse([System.Drawing.Brushes]::White, 3, 3, 5, 5)
Save-Png $bmp $g "UI_Dot"

New-RadialShade "UI_Shade" 512 256 150

$bmp, $g = New-Canvas 240 136
for ($y = 0; $y -lt 136; $y++) {
	for ($x = 0; $x -lt 240; $x++) {
		$dx = ($x + 0.5) / 120.0 - 1.0
		$dy = ($y + 0.5) / 68.0 - 1.0
		$d = [math]::Sqrt($dx * $dx * 0.85 + $dy * $dy * 0.85)
		$v = [math]::Min(1.0, [math]::Max(0.0, ($d - 0.55) / 0.6))
		$a = [int](255 * $v * $v)
		$bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb($a, 255, 255, 255))
	}
}
Save-Png $bmp $g "UI_Vignette"

$bmp, $g = New-Canvas 300 46
$g.FillPath((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(170, 12, 14, 20))), (New-RoundPath 0 0 299 45 22.5))
$g.DrawPath((New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(70, 255, 255, 255)), 1.5), (New-RoundPath 1 1 297 43 21.5))
Save-Png $bmp $g "UI_Pill"

$bmp, $g = New-Canvas 256 256
$g.DrawEllipse((New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(130, 0, 0, 0)), 20), 30, 30, 196, 196)
Save-Png $bmp $g "UI_RingBack"

$bmp, $g = New-Canvas 256 256
$g.DrawArc((New-Object System.Drawing.Pen ([System.Drawing.Color]::White), 14), 30, 30, 196, 196, -90 - 15, 30)
Save-Png $bmp $g "UI_RingSegment"
