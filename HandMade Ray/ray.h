#pragma once
#include "raymath.h"
#define Assert assert
typedef float f32;


struct image_u32
{
	u32 Width;
	u32 Height;
	u32* Pixels;
};

#define F32MAX FLT_MAX;
#define F32MIN FLT_MIN; 
#define Pi32 3.15159265359f
#define Tau32 6.28318530717958647692f

#define internal static
#define global static 

struct material
{
	f32 Scatter; // Note(casey) : 0 is pure diffuse("chalk") , 1 is pure specular("mirror);
	v3 EmitColor;
	v3 RefColor;

};

// TODO(casey): These will probably be reorganized for SIMD :)
struct plane
{
	u32 MatIndex; // Material Index;
	v3 N; // normal
	f32 d; // distance alone the normal?
};

struct sphere
{
	v3 P; // center
	f32 r; // radius
	u32 MatIndex;
};

struct world
{ 
	u32 MaterialCount;
	material* Materials;

	u32 PlaneCount;
	plane* Planes;

	u32 SphereCount;
	sphere* Spheres;

};

// 记录每个Tile的信息
struct work_order
{
	world* World;
	image_u32 Image;
	u32 XMin;
	u32 YMin;
	u32 OnePastXMax;
	u32 OnePastYMax;
};
// 队列存储了上面的Order
struct work_queue
{
	u32 WorkOrderCount;
	work_order* WorkOrders;

	volatile u64 NextWorkOrderIndex;
	volatile u64 BouncesComputed;
	volatile u64 TileRetiredCount;
};