#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "ray.h"

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef s32 b32;
typedef s32 b32x;

typedef float f32;

#define internal static


#pragma pack(push, 1)
struct bitmap_header
{
	u16 FileType;
	u32 FileSize;
	u16 Reserved1;
	u16 Reserved2;
	u32 BitmapOffset;
	u32 Size;
	s32 Width;
	s32 Height;
	u16 Planes;
	u16 BitsPerPixel;
	u32 Compression;
	u32 SizeofBitmap;
	s32 HorzResolution;
	s32 VertResolution;
	u32 ColorsUsed;
	u32 ColorsImporttant;
}; 
#pragma pack(pop)

struct image_u32
{
	u32 Width;
	u32 Height;
	u32* Pixels;
};

internal u32 GetTotalPixelSize(image_u32 Image)
{
	u32 Result = Image.Width * Image.Height * sizeof(u32);
	return Result;
}

internal image_u32 AllocateImage(u32 Width, u32 Height)
{
	image_u32 Image = {};
	Image.Width = Width;
	Image.Height = Height;

	u32 OutputPixelSize = sizeof(u32) * GetTotalPixelSize(Image);
	Image.Pixels = (u32*)malloc(OutputPixelSize);

	return Image;
}

internal v3 RayCast(world* World, v3 RayOrigin, v3 RayDirection)
{
	v3 Result = World->Materials[0].Color; // Black

	f32 HitDistance = F32MAX;

	for (u32 PlaneIndex = 0; PlaneIndex < World->PlaneCount; PlaneIndex++)
	{
		plane Plane = World->Planes[PlaneIndex];

		f32 Tolerance = 0.0001f;
		f32 Denom = Inner(RayDirection, Plane.N); // 用于判断 射线与平面平行的标志
		if ((Denom < -Tolerance) || (Denom > Tolerance)) { // 判断是否平行
			f32 ThisDistance = (-Plane.d - Inner(RayOrigin, Plane.N)) / Denom;

			if ((ThisDistance > 0) && (ThisDistance < HitDistance)) // 反方向的distance砍掉， 因为射线不可能反方向击中物体
			{
				HitDistance = ThisDistance;
				Result = World->Materials[Plane.MatIndex].Color;
			}
		}
	}

	return Result;
}

internal void WriteImage(image_u32 Image, const char* OutputFileName)
{
	u32 OutputPixelSize = GetTotalPixelSize(Image);

	bitmap_header Header = {};
	Header.FileType = 0x4D42; // 固有格式
	Header.FileSize = sizeof(Header) * OutputPixelSize;
	Header.BitmapOffset = sizeof(Header);
	Header.Size = sizeof(Header) - 14;
	Header.Width = Image.Width;
	Header.Height = Image.Height;
	Header.Planes = 1;
	Header.BitsPerPixel = 32; 
	Header.Compression = 0;
	Header.SizeofBitmap = OutputPixelSize;
	Header.HorzResolution = 0;
	Header.VertResolution = 0;
	Header.ColorsUsed = 0;
	Header.ColorsImporttant = 0;


	FILE* OutFile = fopen(OutputFileName, "wb");
	if (OutFile)
	{
		fwrite(&Header, sizeof(Header), 1, OutFile);
		fwrite(Image.Pixels, OutputPixelSize, 1, OutFile);
		fclose(OutFile);
	}
	else
	{
		fprintf(stderr, "[ERROR] Unable to write output file %s.\n", OutputFileName);
	}
}

int main(int argc, char** argv)
{
	printf("Raycastin' ...\n");


	material Materials[2] = {};
	Materials[0].Color = V3(0, 0, 0);
	Materials[1].Color = V3(1, 0, 0);

	plane Plane = {};
	Plane.N = V3(0, 0, 1); // 这里假设Z轴向上
	Plane.d = 0;
	Plane.MatIndex = 1;

	world World = {};
	World.MaterialCount = 2;
	World.Materials = Materials;
	World.PlaneCount = 1;
	World.Planes = &Plane;
	World.SphereCount = 0;
	World.Spheres = 0;

	//                      (0, 0, 1)
	//			  y'       /\
	//			      \     |    x'
	//			       \    |   /
	//                   \  |  / 
	//                    .Cam 
	//				           \
	//                          \
 	//                           \
	//			                  z
	//                
	//       z
	//   y   |
	//    \  |
	//     \ |
	//      .target ---->x
	// 
	// 
	//
	v3 CameraP = V3(0, -10, 1); // camera pos
	v3 CameraZ = NOZ(CameraP - V3(0, 0, 0)); // camera ray  NOZ means normalize   z' (0, 1, 0)
	v3 CameraX = NOZ(Cross(V3(0, 0, 1), CameraZ));  // x' 
	v3 CameraY = NOZ(Cross(CameraZ, CameraX));    // y'

	u32 OutputWidth = 1280;
	u32 OutputHeight = 720;
	image_u32 Image = AllocateImage(1280, 720);

	f32 FilmDist = 1.0f;
	f32 FilmW = 1.0f;
	f32 FilmH = 1.0f;
	f32 HalfFilmW = 0.5f * FilmW;
	f32 HalfFilmH = 0.5f * FilmH;
	v3 FilmCenter = CameraP - FilmDist * CameraZ;

	u32* Out = Image.Pixels;
	for (u32 Y = 0; Y < Image.Height; ++Y)
	{
		f32 FilmY = -1.0f + 2.0f * ((f32) Y / (f32) Image.Height);
		for (u32 X = 0; X < Image.Width; ++X)
		{
			f32 FilmX = -1.0f + 2.0f * ((f32)X / (f32)Image.Width);

			v3 FilmP = FilmCenter + FilmX * HalfFilmW * CameraX + FilmY * HalfFilmH * CameraY;
			
			v3 RayOrigin = CameraP;
			v3 RayDirection = NOZ(FilmP - CameraP);
			
			v3 Color = RayCast(&World, RayOrigin, RayDirection);
			
			v4 BMPColor = V4(255.0f * Color, 255.0f);
			u32 BMPValue = BGRAPack4x8(BMPColor);


			*Out++ =  BMPValue; // (Y < 32) ? 0xFFFF0000 : 0xFF0000FF;
		}
	}


	WriteImage(Image, "test.bmp");

	printf("Done...\n");

	return 0;
}