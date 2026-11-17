$root = Split-Path -Parent $PSScriptRoot
$outDir = Join-Path $root "MaxData\textures\boss"
New-Item -ItemType Directory -Force $outDir | Out-Null

$fbm = Join-Path $env:USERPROFILE "Downloads\Tactical_Soldier_02.Fbx(1).fbm"
$gunTex = Join-Path $env:USERPROFILE "Downloads\m4-carabine\textures"
$copyList = @(
	@($fbm, "Std_Skin_Head_Diffuse.jpg"), @($fbm, "Std_Skin_Head_Normal.png"),
	@($fbm, "Std_Skin_Body_Diffuse.jpg"), @($fbm, "Std_Skin_Body_Normal.png"),
	@($fbm, "Std_Skin_Arm_Diffuse.jpg"), @($fbm, "Std_Skin_Arm_Normal.png"),
	@($fbm, "Std_Skin_Leg_Diffuse.jpg"), @($fbm, "Std_Skin_Leg_Normal.png"),
	@($fbm, "Std_Nails_Diffuse.jpg"), @($fbm, "Std_Nails_Normal.png"),
	@($fbm, "Std_Eye_R_Diffuse.jpg"), @($fbm, "Std_Eye_L_Diffuse.jpg"),
	@($fbm, "Glass_LP2_Diffuse.png"), @($fbm, "Glass_LP2_Normal.png"),
	@($fbm, "Shirt_02_Diffuse.png"), @($fbm, "Shirt_02_Normal.png"),
	@($fbm, "Pants_LP3_Diffuse.png"), @($fbm, "Pants_LP3_Normal.png"),
	@($gunTex, "m4_AR15_BaseColor.png"), @($gunTex, "m4_AR15_Normal.png"),
	@($gunTex, "m4_Magazine_BaseColor.png"), @($gunTex, "m4_Magazine_Normal.png"),
	@($gunTex, "m4_acog_BaseColor.png"), @($gunTex, "m4_acog_Normal.png"),
	@($gunTex, "m4_rail_BaseColor.png"), @($gunTex, "m4_rail_Normal.png"),
	@($gunTex, "m4_suppresor_BaseColor.png"), @($gunTex, "m4_suppresor_Normal.png"),
	@($gunTex, "m4_Front_Grip_BaseColor.png"), @($gunTex, "m4_Front_Grip_Normal.png"),
	@($gunTex, "m4_laser_BaseColor.png"), @($gunTex, "m4_laser_Normal.png")
)
foreach ($item in $copyList) {
	Copy-Item (Join-Path $item[0] $item[1]) (Join-Path $outDir ("Boss_" + $item[1])) -Force
}

Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

public static class BossTextureMaker
{
	const int SIZE = 512;

	static double Hash(int x, int y, int s)
	{
		unchecked
		{
			uint h = (uint)(x * 374761393 + y * 668265263 + s * 1274126177);
			h = (h ^ (h >> 13)) * 1274126177;
			h ^= h >> 16;
			return (h & 0xFFFFFF) / 16777215.0;
		}
	}

	static double Noise(double x, double y, int s)
	{
		int ix = (int)Math.Floor(x);
		int iy = (int)Math.Floor(y);
		double fx = x - ix;
		double fy = y - iy;
		fx = fx * fx * (3.0 - 2.0 * fx);
		fy = fy * fy * (3.0 - 2.0 * fy);
		double a = Hash(ix, iy, s) + (Hash(ix + 1, iy, s) - Hash(ix, iy, s)) * fx;
		double b = Hash(ix, iy + 1, s) + (Hash(ix + 1, iy + 1, s) - Hash(ix, iy + 1, s)) * fx;
		return a + (b - a) * fy;
	}

	static double Fbm(double x, double y, int s)
	{
		double sum = 0.0;
		double amp = 0.5;
		double total = 0.0;
		for (int i = 0; i < 4; i++)
		{
			sum += Noise(x, y, s + i) * amp;
			total += amp;
			x *= 2.03;
			y *= 2.03;
			amp *= 0.5;
		}
		return sum / total;
	}

	static int ToByte(double v)
	{
		return (int)Math.Max(0.0, Math.Min(255.0, v * 255.0));
	}

	public static void Make(string path, double r, double g, double b, double noise, double weave, double stripe, int seed)
	{
		var bmp = new Bitmap(SIZE, SIZE, PixelFormat.Format32bppArgb);
		var data = bmp.LockBits(new Rectangle(0, 0, SIZE, SIZE), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
		var bytes = new byte[SIZE * SIZE * 4];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				double n = Fbm(x / 48.0, y / 48.0, seed) - 0.5;
				double f = 1.0 + n * noise;
				if (weave > 0.0)
				{
					double w = ((x / 4) + (y / 4)) % 2 == 0 ? 1.0 : 1.0 - weave;
					f *= w;
				}
				if (stripe > 0.0 && ((x + y) / 40) % 2 == 0 && y > SIZE * 0.82)
				{
					f *= 1.0 + stripe;
				}
				int i = (y * SIZE + x) * 4;
				bytes[i + 0] = (byte)ToByte(b * f);
				bytes[i + 1] = (byte)ToByte(g * f);
				bytes[i + 2] = (byte)ToByte(r * f);
				bytes[i + 3] = 255;
			}
		}
		Marshal.Copy(bytes, 0, data.Scan0, bytes.Length);
		bmp.UnlockBits(data);
		bmp.Save(path, ImageFormat.Png);
		bmp.Dispose();
	}
}
'@

[BossTextureMaker]::Make((Join-Path $outDir "Boss_GearOlive.png"), 0.30, 0.31, 0.20, 0.35, 0.08, 0.0, 11)
[BossTextureMaker]::Make((Join-Path $outDir "Boss_GearBlack.png"), 0.11, 0.11, 0.12, 0.40, 0.06, 0.0, 23)
[BossTextureMaker]::Make((Join-Path $outDir "Boss_Grenade.png"), 0.24, 0.27, 0.16, 0.30, 0.0, 0.0, 37)
[BossTextureMaker]::Make((Join-Path $outDir "Boss_Jetpack.png"), 0.26, 0.27, 0.29, 0.30, 0.0, 0.0, 41)
Get-ChildItem $outDir | Select-Object Name, Length
