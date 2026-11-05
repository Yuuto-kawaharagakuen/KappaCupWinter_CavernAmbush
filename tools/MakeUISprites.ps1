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

$bmp, $g = New-Canvas 1920 1080
$cx = 960; $cy = 540; $r = 470
$mask = New-Object System.Drawing.Drawing2D.GraphicsPath
$mask.FillMode = [System.Drawing.Drawing2D.FillMode]::Alternate
$mask.AddRectangle((New-Object System.Drawing.RectangleF 0, 0, 1920, 1080))
$mask.AddEllipse($cx - $r, $cy - $r, $r * 2, $r * 2)
$g.FillPath((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 0, 0, 0))), $mask)
for ($i = 0; $i -lt 40; $i++) {
	$a = [int](150 * (1.0 - $i / 40.0) * (1.0 - $i / 40.0))
	$rr = $r - $i
	$g.DrawEllipse((New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb($a, 0, 0, 0)), 1.5), $cx - $rr, $cy - $rr, $rr * 2, $rr * 2)
}
$thin = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(230, 0, 0, 0)), 2
$thick = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(240, 0, 0, 0)), 8
$g.DrawLine($thin, $cx - $r, $cy, $cx + $r, $cy)
$g.DrawLine($thin, $cx, $cy - $r, $cx, $cy + $r)
$g.DrawLine($thick, $cx - $r, $cy, $cx - 170, $cy)
$g.DrawLine($thick, $cx + 170, $cy, $cx + $r, $cy)
$g.DrawLine($thick, $cx, $cy + 170, $cx, $cy + $r)
for ($i = 1; $i -le 4; $i++) {
	$d = $i * 36
	$g.FillEllipse((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(230, 0, 0, 0))), $cx + $d - 3, $cy - 3, 6, 6)
	$g.FillEllipse((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(230, 0, 0, 0))), $cx - $d - 3, $cy - 3, 6, 6)
	$g.FillEllipse((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(230, 0, 0, 0))), $cx - 3, $cy + $d - 3, 6, 6)
}
$g.FillEllipse((New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 255, 40, 30))), $cx - 4, $cy - 4, 8, 8)
Save-Png $bmp $g "UI_Scope"