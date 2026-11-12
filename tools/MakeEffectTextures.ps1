Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

public static class EffectTextureMaker
{
	const int SIZE = 512;

	static double Hash(int x, int s)
	{
		unchecked
		{
			uint h = (uint)(x * 374761393 + s * 1274126177);
			h = (h ^ (h >> 13)) * 1274126177;
			h ^= h >> 16;
			return (h & 0xFFFFFF) / 16777215.0;
		}
	}

	static double Clamp(double v)
	{
		return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
	}

	static double Lerp(double a, double b, double t)
	{
		return a + (b - a) * t;
	}

	static void Put(byte[] p, int x, int y, double intensity)
	{
		double t = Clamp(intensity);
		double r = Clamp(t * 1.6) * 255.0;
		double g = Clamp(t * 1.25 - 0.08) * 235.0;
		double b = Clamp(t * 1.6 - 0.75) * 210.0;
		int i = (y * SIZE + x) * 3;
		p[i] = (byte)b;
		p[i + 1] = (byte)g;
		p[i + 2] = (byte)r;
	}

	static void Save(string path, byte[] p)
	{
		using (Bitmap bmp = new Bitmap(SIZE, SIZE, PixelFormat.Format24bppRgb))
		{
			BitmapData data = bmp.LockBits(new Rectangle(0, 0, SIZE, SIZE), ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);
			for (int y = 0; y < SIZE; y++)
			{
				Marshal.Copy(p, y * SIZE * 3, new IntPtr(data.Scan0.ToInt64() + y * data.Stride), SIZE * 3);
			}
			bmp.UnlockBits(data);
			bmp.Save(path, ImageFormat.Png);
		}
	}

	public static void Star(string path)
	{
		const int SPIKES = 7;
		byte[] p = new byte[SIZE * SIZE * 3];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				double dx = (x + 0.5) / SIZE * 2.0 - 1.0;
				double dy = (y + 0.5) / SIZE * 2.0 - 1.0;
				double r = Math.Sqrt(dx * dx + dy * dy);
				double a = Math.Atan2(dy, dx);
				double sector = (a / (Math.PI * 2.0) + 0.5) * SPIKES;
				int index = (int)Math.Floor(sector);
				double local = sector - index;
				double length = Lerp(0.55, 0.95, Hash(index % SPIKES, 7));
				double spike = Math.Pow(1.0 - Math.Abs(local - 0.5) * 2.0, 6.0) * Clamp(1.0 - r / length);
				double core = Math.Exp(-r * r * 30.0);
				double glow = Math.Exp(-r * 4.5) * 0.45;
				Put(p, x, y, core + spike * 0.9 + glow * Clamp(1.0 - r));
			}
		}
		Save(path, p);
	}

	public static void Flame(string path)
	{
		byte[] p = new byte[SIZE * SIZE * 3];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				double u = (x + 0.5) / SIZE * 2.0 - 1.0;
				double v = (y + 0.5) / SIZE * 2.0 - 1.0;
				double along = Clamp(1.0 - Math.Abs(v));
				double width = Lerp(0.08, 0.55, Math.Sqrt(along));
				double body = Math.Exp(-(u * u) / (width * width) * 2.5) * along * along;
				double streak = 0.8 + 0.2 * Math.Sin(u * 22.0 + Math.Sin(v * 5.0) * 2.0);
				double core = Math.Exp(-(u * u) * 60.0) * along * 0.35;
				Put(p, x, y, (body * streak + core) * 1.15);
			}
		}
		Save(path, p);
	}
}
'@

$root = Split-Path -Parent $PSScriptRoot
$srcDir = Join-Path $root "MaxData\textures\effects"
$outDir = Join-Path $root "Game\Assets\modelData"
$texconv = "C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\texconv.exe"
New-Item -ItemType Directory -Force $srcDir | Out-Null

$star = Join-Path $srcDir "FX_MuzzleStar.png"
$flame = Join-Path $srcDir "FX_MuzzleFlame.png"
[EffectTextureMaker]::Star($star)
[EffectTextureMaker]::Flame($flame)

foreach ($path in @($star, $flame)) {
	& $texconv -vflip -f BC7_UNORM -srgb -y -w 256 -h 256 -o $outDir $path | Out-Null
}
