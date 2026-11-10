Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

public static class PropTextureMaker
{
	const int SIZE = 1024;
	const double BARREL_HEIGHT = 95.6;

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

	static double Lerp(double a, double b, double t)
	{
		return a + (b - a) * t;
	}

	static double Clamp(double v)
	{
		return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
	}

	static double Noise(double x, double y, int s)
	{
		int ix = (int)Math.Floor(x);
		int iy = (int)Math.Floor(y);
		double fx = x - ix;
		double fy = y - iy;
		fx = fx * fx * (3.0 - 2.0 * fx);
		fy = fy * fy * (3.0 - 2.0 * fy);
		double a = Lerp(Hash(ix, iy, s), Hash(ix + 1, iy, s), fx);
		double b = Lerp(Hash(ix, iy + 1, s), Hash(ix + 1, iy + 1, s), fx);
		return Lerp(a, b, fy);
	}

	static double Fbm(double x, double y, int s, int octaves)
	{
		double sum = 0.0;
		double amp = 0.5;
		double total = 0.0;
		for (int i = 0; i < octaves; i++)
		{
			sum += Noise(x, y, s + i) * amp;
			total += amp;
			x *= 2.03;
			y *= 2.03;
			amp *= 0.5;
		}
		return sum / total;
	}

	static void Mix(double[] c, double r, double g, double b, double t)
	{
		c[0] = Lerp(c[0], r, t);
		c[1] = Lerp(c[1], g, t);
		c[2] = Lerp(c[2], b, t);
	}

	static void Scale(double[] c, double k)
	{
		c[0] *= k;
		c[1] *= k;
		c[2] *= k;
	}

	static void Wood(double[] c, double x, double y, int board)
	{
		double n = Fbm(x * 0.02 + board * 13.7, y * 0.0035 + board * 5.1, 11, 4);
		double warp = Fbm(x * 0.01 + board * 3.3, y * 0.003, 31, 2);
		double grain = 0.5 + 0.5 * Math.Sin(x * 0.22 + n * 18.0 + warp * 8.0);
		double fine = Fbm(x * 0.25 + board * 7.1, y * 0.012, 41, 3);
		double t = 0.35 * grain + 0.45 * fine + 0.2 * n;
		double tone = (Hash(board, 7, 99) - 0.5) * 0.28;
		t = Clamp((t - 0.5) * 1.5 + 0.5 + tone);
		c[0] = Lerp(50.0, 132.0, t);
		c[1] = Lerp(38.0, 108.0, t);
		c[2] = Lerp(31.0, 88.0, t);
		double dirt = Fbm(x * 0.006 + 50.0, y * 0.006, 61, 3);
		Scale(c, Lerp(0.72, 1.08, dirt));
	}

	static void BoardEdge(double[] c, double edge, double gap, double bevel)
	{
		if (edge < gap)
		{
			c[0] = 18.0;
			c[1] = 14.0;
			c[2] = 12.0;
		}
		else if (edge < gap + bevel)
		{
			Scale(c, Lerp(0.58, 1.0, (edge - gap) / bevel));
		}
	}

	static void Metal(double[] c, double x, double y, double brightness)
	{
		double n = Fbm(x * 0.02, y * 0.02, 71, 4);
		double pits = Fbm(x * 0.12, y * 0.12, 81, 3);
		double t = Clamp((0.55 * n + 0.45 * pits - 0.5) * 2.2 + 0.5);
		c[0] = Lerp(50.0, 150.0, t) * brightness;
		c[1] = Lerp(50.0, 150.0, t) * brightness;
		c[2] = Lerp(53.0, 155.0, t) * brightness;
		double rust = Fbm(x * 0.03 + 100.0, y * 0.03, 91, 4);
		if (rust > 0.64)
		{
			Mix(c, 88.0, 62.0, 44.0, Clamp((rust - 0.64) * 6.0) * 0.6);
		}
	}

	static void Put(byte[] p, int stride, int x, int y, double[] c)
	{
		int i = y * stride + x * 3;
		p[i] = (byte)Math.Max(0.0, Math.Min(255.0, c[2]));
		p[i + 1] = (byte)Math.Max(0.0, Math.Min(255.0, c[1]));
		p[i + 2] = (byte)Math.Max(0.0, Math.Min(255.0, c[0]));
	}

	static void Save(string path, byte[] p, int stride)
	{
		using (Bitmap bmp = new Bitmap(SIZE, SIZE, PixelFormat.Format24bppRgb))
		{
			BitmapData data = bmp.LockBits(new Rectangle(0, 0, SIZE, SIZE), ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);
			for (int y = 0; y < SIZE; y++)
			{
				Marshal.Copy(p, y * stride, new IntPtr(data.Scan0.ToInt64() + y * data.Stride), SIZE * 3);
			}
			bmp.UnlockBits(data);
			bmp.Save(path, ImageFormat.Png);
		}
	}

	public static void Crate(string path)
	{
		const int WOOD_WIDTH = 768;
		const int BOARD_WIDTH = 128;
		int stride = SIZE * 3;
		byte[] p = new byte[stride * SIZE];
		double[] c = new double[3];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				if (x >= WOOD_WIDTH)
				{
					Metal(c, x, y, 1.0);
				}
				else
				{
					int board = x / BOARD_WIDTH;
					double local = x - board * BOARD_WIDTH;
					Wood(c, x, y, board);
					double jointY = 256.0 + 180.0 + Hash(board, 3, 17) * 400.0;
					double jointDist = Math.Abs(y - jointY);
					if (jointDist < 2.0)
					{
						c[0] = 22.0;
						c[1] = 17.0;
						c[2] = 14.0;
					}
					else if (jointDist < 8.0)
					{
						Scale(c, Lerp(0.66, 1.0, (jointDist - 2.0) / 6.0));
					}
					double centerX = BOARD_WIDTH * 0.5;
					for (int side = -1; side <= 1; side += 2)
					{
						double nx = local - (centerX + side * 32.0);
						for (int row = -1; row <= 1; row += 2)
						{
							double ny = y - (jointY + row * 16.0);
							double d = Math.Sqrt(nx * nx + ny * ny);
							if (d < 4.5)
							{
								Mix(c, 34.0, 32.0, 34.0, 1.0);
							}
							else if (d < 8.0)
							{
								Scale(c, Lerp(0.72, 1.0, (d - 4.5) / 3.5));
							}
						}
					}
					BoardEdge(c, Math.Min(local, BOARD_WIDTH - 1 - local), 3.0, 9.0);
				}
				Put(p, stride, x, y, c);
			}
		}
		Save(path, p, stride);
	}

	static bool InHoop(double z)
	{
		return z <= 3.9 || (z >= 43.6 && z <= 52.0) || z >= 91.8;
	}

	static double HoopDistance(double z)
	{
		double a = Math.Abs(z - 3.9);
		double b = Math.Min(Math.Abs(z - 43.6), Math.Abs(z - 52.0));
		double d = Math.Abs(z - 91.8);
		return Math.Min(a, Math.Min(b, d));
	}

	public static void Barrel(string path)
	{
		const int SIDE_WIDTH = 768;
		const int STAVE_WIDTH = 64;
		const int CAP_TOP = 768;
		const int CAP_BOARD = 48;
		int stride = SIZE * 3;
		byte[] p = new byte[stride * SIZE];
		double[] c = new double[3];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				if (x < SIDE_WIDTH)
				{
					double v = (SIZE - y - 0.5) / SIZE;
					double z = (v - 0.005) / 0.99 * BARREL_HEIGHT;
					double hoopDist = HoopDistance(z);
					if (InHoop(z))
					{
						Metal(c, x * 1.5, y, 0.68);
						if (hoopDist < 0.4)
						{
							Scale(c, 0.6);
						}
					}
					else
					{
						int stave = x / STAVE_WIDTH;
						double local = x - stave * STAVE_WIDTH;
						Wood(c, x * 2.0, y, stave + 20);
						BoardEdge(c, Math.Min(local, STAVE_WIDTH - 1 - local), 2.0, 6.0);
						if (hoopDist < 2.2)
						{
							Scale(c, Lerp(0.5, 1.0, hoopDist / 2.2));
						}
					}
				}
				else if (y >= CAP_TOP)
				{
					int board = (x - SIDE_WIDTH) / CAP_BOARD;
					double local = (x - SIDE_WIDTH) - board * CAP_BOARD;
					Wood(c, x * 2.0, y * 3.0, board + 40);
					BoardEdge(c, Math.Min(local, CAP_BOARD - 1 - local), 1.5, 4.0);
				}
				else
				{
					Metal(c, x, y, 0.68);
				}
				Put(p, stride, x, y, c);
			}
		}
		Save(path, p, stride);
	}

	public static void Hole(string path)
	{
		const double DISC_RATE = 0.75;
		int stride = SIZE * 3;
		byte[] p = new byte[stride * SIZE];
		double[] c = new double[3];
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				double dx = (x + 0.5) / SIZE * 2.0 - 1.0;
				double dy = (y + 0.5) / SIZE * 2.0 - 1.0;
				double t = Clamp((Math.Sqrt(dx * dx + dy * dy) / DISC_RATE - 0.45) / 0.55);
				double n = Fbm(x * 0.03, y * 0.03, 111, 4);
				t = Clamp(t * t + (n - 0.5) * 0.35 * t);
				c[0] = Lerp(5.0, 74.0, t);
				c[1] = Lerp(4.0, 58.0, t);
				c[2] = Lerp(3.0, 41.0, t);
				Put(p, stride, x, y, c);
			}
		}
		Save(path, p, stride);
	}
}
'@

$root = Split-Path -Parent $PSScriptRoot
$srcDir = Join-Path $root "MaxData\textures\props"
$outDir = Join-Path $root "Game\Assets\modelData"
$texconv = "C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\texconv.exe"
New-Item -ItemType Directory -Force $srcDir | Out-Null

$crate = Join-Path $srcDir "Prop_Crate.png"
$barrel = Join-Path $srcDir "Prop_WoodBarrel.png"
[PropTextureMaker]::Crate($crate)
[PropTextureMaker]::Barrel($barrel)
$hole = Join-Path $srcDir "T_Hole.png"
[PropTextureMaker]::Hole($hole)

foreach ($path in @($crate, $barrel, $hole)) {
	& $texconv -vflip -f BC7_UNORM -srgb -y -w 1024 -h 1024 -o $outDir $path | Out-Null
}
