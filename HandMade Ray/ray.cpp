/* TODO(casey):
*  this is only the very barest of bones for raytracer! We are computing
* things inaccurately and physically incorrect _everywhere_. We'll 
* fix it some day when we coma back to it :)

*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include "ray.h"
#include <windows.h>


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

internal u32 GetCpuCoreCount()
{
	SYSTEM_INFO Info;
	GetSystemInfo(&Info);
	u32 Result = Info.dwNumberOfProcessors;
	return Result;
}

internal u32* GetPixelPointer(image_u32 Image, u32 X, u32 Y)
{
	u32* Result = Image.Pixels + Y * Image.Width + X;
	return Result;
}

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

internal f32 RandomUnilateral()
{
	// TODO(casey) : _MUST_ replace this with better entropy later

	f32 Result =  (f32)rand() / (f32)RAND_MAX;
	return Result;
}

internal f32 RamdomBilateral()
{
	f32 Result =  -1.0f + 2.0f * RandomUnilateral();
	return Result;
}


internal f32 ExactLinearTosRGB(f32 L)
{
	if (L < 0.0f)
	{
		L = 0.0f;
	}
	else if (L > 1.0f)
	{
		L = 1.0f;
	}

	f32 S = L * 12.92f;
	if (L > 0.0031308f)
	{
		S = 1.055f * Pow(L, 1.0f / 2.4f) - 0.055f;
	}

	return S;
}
internal u64 LockedAndReturnPreviousValue(u64 volatile* Value, u64 Addend)
{
	u64 Result = *Value;
	*Value += Addend;
	return Result;
}



internal b32x RenderTile(work_queue* Queue)
{
	u64 WorkOrderIndex = LockedAndReturnPreviousValue(&Queue->NextWorkOrderIndex, 1);
	if (WorkOrderIndex >= Queue->WorkOrderCount)
	{
		return false;
	}
	// WorkOrders ��ʾ�������еĶ�ͷԪ��
	// WorkOrderIndex��ʾ��ǰλ�þ����ͷ����ƫ��
	// �Ӷ�����ȡ��Ԫ��
	work_order* Order = Queue->WorkOrders + WorkOrderIndex;
	world* World = Order->World;
	image_u32 Image = Order->Image;
	u32 XMin = Order->XMin;
	u32 YMin = Order->YMin;
	u32 OnePastXMax = Order->OnePastXMax;
	u32 OnePastYMax = Order->OnePastYMax;


	v3 CameraP = V3(0, -10, 1); // camera pos
	v3 CameraZ = NOZ(CameraP - V3(0, 0, 0)); // camera ray  NOZ means normalize   z' (0, 1, 0)
	v3 CameraX = NOZ(Cross(V3(0, 0, 1), CameraZ));  // x' 
	v3 CameraY = NOZ(Cross(CameraZ, CameraX));    // y'


	f32 FilmDist = 1.0f;
	f32 FilmW = 1.0f;
	f32 FilmH = 1.0f;

	if (Image.Width > Image.Height)
	{
		// ���FilmW = 1.0�Ļ�����ô
		FilmH = (f32)Image.Height * FilmW / Image.Width;
	}
	else if (Image.Height > Image.Width)
	{
		// ���FilmH = 1.0f�Ļ��� ��ô
		FilmW = (f32)Image.Width * FilmH / Image.Height;
	}

	f32 HalfFilmW = 0.5f * FilmW;
	f32 HalfFilmH = 0.5f * FilmH;
	v3 FilmCenter = CameraP - FilmDist * CameraZ;

	f32 HalfPixW = 0.5f / Image.Width;
	f32 HalfPixH = 0.5f / Image.Height;
	u64 BouncesComputed = 0;

	u32 RaysPerPixel = Queue->RaysPerPixel;
	u32 MaxBounceCount = Queue->MaxBounceCount;

#pragma region TileRender
	for (u32 Y = YMin; Y < OnePastYMax; ++Y)
	{
		u32* Out = GetPixelPointer(Image, XMin, Y);

		f32 FilmY = -1.0f + 2.0f * ((f32)Y / (f32)Image.Height);
		for (u32 X = XMin; X < OnePastXMax; ++X)
		{
			f32 FilmX = -1.0f + 2.0f * ((f32)X / (f32)Image.Width);

			v3 FinalColor = {};
			f32 Contrib = 1.0f / (f32)RaysPerPixel;
			for (u32 RayIndex = 0; RayIndex < RaysPerPixel; RayIndex++)
			{
				f32 OffX = FilmX + RamdomBilateral() * HalfPixW;
				f32 OffY = FilmY + RamdomBilateral() * HalfPixH;
				v3 FilmP = FilmCenter + OffX * HalfFilmW * CameraX + OffY * HalfFilmH * CameraY;

				v3 RayOrigin = CameraP;
				v3 RayDirection = NOZ(FilmP - CameraP);

#pragma region RayTracing
				v3 Sample = {};
				v3 Attenuation = V3(1, 1, 1);

				f32 MinHitDistance = 0.001f;

				f32 Tolerance = 0.0001f;

				// TODO(casey): Monte-carlo termination?
				for (u32 BounceCount = 0; BounceCount < MaxBounceCount; BounceCount++)
				{
					f32 HitDistance = F32MAX;
					u32 HitMatIndex = 0;
					//v3 NextOrigin = {};
					++BouncesComputed;

					v3 NextNormal = {};
					for (u32 PlaneIndex = 0; PlaneIndex < World->PlaneCount; PlaneIndex++)
					{
						plane Plane = World->Planes[PlaneIndex];

						f32 Denom = Inner(RayDirection, Plane.N); // �����ж� ������ƽ��ƽ�еı�־
						if ((Denom < -Tolerance) || (Denom > Tolerance)) { // �ж��Ƿ�ƽ��
							f32 t = (-Plane.d - Inner(RayOrigin, Plane.N)) / Denom;

							if ((t > MinHitDistance) && (t < HitDistance)) // �������distance������ ��Ϊ���߲����ܷ������������
							{
								HitDistance = t;
								HitMatIndex = Plane.MatIndex;

								//NextOrigin = RayOrigin + t * RayDirection;
								NextNormal = Plane.N;
							}
						}
					}


					for (u32 SphereIndex = 0; SphereIndex < World->SphereCount; SphereIndex++)
					{
						sphere Sphere = World->Spheres[SphereIndex];

						v3 SphereRelativeRayOrigin = RayOrigin - Sphere.P;
						f32 a = Inner(RayDirection, RayDirection);
						f32 b = 2 * Inner(SphereRelativeRayOrigin, RayDirection);
						f32 c = Inner(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Sphere.r * Sphere.r;
						f32 RootTerm = SquareRoot(b * b - 4.0f * a * c);
						f32 Denom = 2.0f * a;

						if (RootTerm > Tolerance) { // ��� delta < 0 ��ô�޽�
							f32 tp = (-b + RootTerm) / Denom;
							f32 tn = (-b - RootTerm) / Denom;

							f32 t = tp;
							if (tn > MinHitDistance && tn < tp) t = tn; // �������һ������С

							if ((t > MinHitDistance) && (t < HitDistance)) // �������distance������ ��Ϊ���߲����ܷ������������
							{
								HitDistance = t;
								HitMatIndex = Sphere.MatIndex;
								//NextOrigin = RayOrigin + RayDirection * t;
								NextNormal = NOZ(t * RayDirection + RayOrigin - Sphere.P);
							}

						}
					}

					if (HitMatIndex)
					{
						material Mat = World->Materials[HitMatIndex];

						// TODO(casey) : COSINE!!!
						Sample += Hadamard(Attenuation, Mat.EmitColor);
						f32 CosAtten = Inner(-RayDirection, NextNormal);
						if (CosAtten < 0)
						{
							CosAtten = 0;
						}
						Attenuation = Hadamard(Attenuation, CosAtten * Mat.RefColor);

						RayOrigin += HitDistance * RayDirection;

						// TODO(casey): these are not accurate permutations!;
						v3 PureBounce = RayDirection - 2.0f * Inner(RayDirection, NextNormal) * NextNormal;
						v3 RamdomBounce = NOZ(NextNormal + V3(RamdomBilateral(), RamdomBilateral(), RamdomBilateral()));
						RayDirection = NOZ(Lerp(RamdomBounce, Mat.Scatter, PureBounce));
						//RayDirection = PureBounce;

					}
					else
					{
						material Mat = World->Materials[HitMatIndex];
						Sample += Hadamard(Attenuation, Mat.EmitColor);
						break;
					}
				}

				FinalColor += Contrib * Sample;
			}
			#pragma endregion
			//v3 Color = RayCast(&World, RayOrigin, RayDirection);

			// TODO(casey): Real sRGB here
			v4 BMPColor =
			{
				255.0f * ExactLinearTosRGB(FinalColor.r),
				255.0f * ExactLinearTosRGB(FinalColor.g),
				255.0f * ExactLinearTosRGB(FinalColor.b),
				255.0f,
			};

			u32 BMPValue = BGRAPack4x8(BMPColor);


			*Out++ = BMPValue; // (Y < 32) ? 0xFFFF0000 : 0xFF0000FF;
		}

	}
#pragma endregion
	LockedAndReturnPreviousValue(&Queue->BouncesComputed, BouncesComputed);
	LockedAndReturnPreviousValue(&Queue->TileRetiredCount, 1);
	return true;
	//Queue->BouncesComputed += BouncesComputed;
	//++Queue->TileRetiredCount;
}

internal void WriteImage(image_u32 Image, const char* OutputFileName)
{
	u32 OutputPixelSize = GetTotalPixelSize(Image);

	bitmap_header Header = {};
	Header.FileType = 0x4D42; // ���и�ʽ
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

internal DWORD WINAPI WorkerThread(void* lpParameter)
{
	work_queue* Queue = (work_queue*)lpParameter;
	while (RenderTile(Queue))
	{}
	return 0;
}
internal void CreateWorkThread(void* Parameter)
{
	DWORD ThreadID;
	HANDLE ThreadHandle = CreateThread(0, 0, WorkerThread, Parameter, 0, &ThreadID);
	CloseHandle(ThreadHandle); // �رո��ļ�������
}

int main(int argc, char** argv)
{ 

	//printf("CLOCKS_PER_SEC: %d\n", CLOCKS_PER_SEC);
	material Materials[7] = {};
	Materials[0].EmitColor = V3(0.3f, 0.4f, 0.5f);
	Materials[1].RefColor = V3(0.5f, 0.5f, 0.5f);
	Materials[2].RefColor = V3(0.7f, 0.5f, 0.3f);
	Materials[3].EmitColor = V3(4.0f, 0.0f, 0.0f);
	Materials[4].RefColor = V3(0.4f, 0.8f, 0.2f);
	Materials[4].Scatter = 0.7f;
	Materials[5].RefColor = V3(0.4f, 0.8f, 0.9f);
	Materials[5].Scatter = 0.85f;
	Materials[6].RefColor = V3(0.95f, 0.95f, 0.95f);
	Materials[6].Scatter = 1.0f;


	plane Planes[1]= {};
	Planes[0].N = V3(0, 0, 1); // �������Z������
	Planes[0].d = 0;
	Planes[0].MatIndex = 1;

	sphere Spheres[5] = {};
	Spheres[0].P = V3(0, 0, 0);
	Spheres[0].r = 1.0f;
	Spheres[0].MatIndex = 2;

	Spheres[1].P = V3(3, -2, 0);
	Spheres[1].r = 1.0f;
	Spheres[1].MatIndex = 3;

	Spheres[2].P = V3(-2, -1, 2);
	Spheres[2].r = 1.0f;
	Spheres[2].MatIndex = 4;

	Spheres[3].P = V3(1, -1, 3);
	Spheres[3].r = 1.0f;
	Spheres[3].MatIndex = 5;

	Spheres[4].P = V3(-2, 3, 0);
	Spheres[4].r = 2.0f;
	Spheres[4].MatIndex = 6;

	world World = {};
	World.MaterialCount = _countof(Materials);
	World.Materials = Materials;
	World.PlaneCount = _countof(Planes);
	World.Planes = Planes;
	World.SphereCount = _countof(Spheres);
	World.Spheres = Spheres;

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

	u32 OutputWidth = 1280;
	u32 OutputHeight = 720;
	image_u32 Image = AllocateImage(1280, 720);



	// Performance: 0.000269ms / bounce
	// Performance: 0.000267ms / bounce
	// Performance: 0.000265ms / bounce

#pragma region Image_split_into_Tile
	u32 CoreCount = GetCpuCoreCount();
	u32 TileWidth = Image.Width / CoreCount;
	u32 TileHeight = TileWidth;
	TileHeight = TileWidth = 64;
	u32 TileCountX = (Image.Width + TileWidth - 1) / TileWidth;
	u32 TileCountY = (Image.Height + TileHeight - 1) / TileHeight;
	u32 TotalTileCount = TileCountX * TileCountY;

	u32 TileRetiredCount = 0;
	#if 1
	work_queue Queue = {};
	Queue.WorkOrders = (work_order*)malloc(TotalTileCount * sizeof(work_order));
	Queue.MaxBounceCount = 8;
	Queue.RaysPerPixel = 1024;

	printf("Configuration %d cores with %d %dx%d (%dk/tile) tiles\n", CoreCount, TotalTileCount, TileWidth, TileHeight, TileWidth * TileHeight * 4 / 1024);
	printf("Quality: %d rays/pixel, %d max bounces per ray\n", Queue.RaysPerPixel, Queue.MaxBounceCount);

	for (u32 TileY = 0; TileY < TileCountY; TileY++)
	{
		u32 MinY = TileY * TileHeight;
		u32 OnePastMaxY = MinY + TileHeight;
		if (OnePastMaxY > Image.Height)
		{
			OnePastMaxY = Image.Height;
		}

		for (u32 TileX = 0; TileX < TileCountX; TileX ++ )
		{
		 
			u32 MinX = TileX * TileWidth;
			u32 OnePastMaxX = MinX + TileWidth;
			if (OnePastMaxX > Image.Width)
			{
				OnePastMaxX = Image.Width;
			}

			work_order* Order = Queue.WorkOrders + Queue.WorkOrderCount ++ ;
			Assert(Queue.WorkOrderCount <= TotalTileCount);

			Order->World = &World;
			Order->Image = Image;
			Order->XMin	= MinX;
			Order->YMin = MinY;
			Order->OnePastXMax = OnePastMaxX;
			Order->OnePastYMax = OnePastMaxY;
		}

	}

	Assert(Queue.WorkOrderCount == TotalTileCount);
	// memory fence
	LockedAndReturnPreviousValue(&Queue.NextWorkOrderIndex, 0);

	clock_t StartClock = clock();
	for (u32 CoreIndex = 1; CoreIndex < CoreCount; CoreIndex++)
	{
		CreateWorkThread(&Queue);
	}

	while (Queue.TileRetiredCount < TotalTileCount)
	{ 

		if (RenderTile(&Queue))
		{ 
			printf("\rRaycasting row %d%% ...\n", 100 * (u32)Queue.TileRetiredCount / TotalTileCount);
			fflush(stdout);
		}
	}
	#endif 
#pragma endregion
	//RenderTile(&World, Image, 0, 0, Image.Width, Image.Height);

	clock_t EndClock = clock();
	clock_t TimeElapsed = EndClock - StartClock;
	printf("\nRaycasting time = %dms\n", TimeElapsed);
	printf("\nTotal bounces: %llu\n", Queue.BouncesComputed);
	printf("\nPerformance: %fms/bounce\n", (f64)TimeElapsed / (f64)Queue.BouncesComputed);
	WriteImage(Image, "test.bmp");

	printf("\nDone...\n");

	return 0;
}