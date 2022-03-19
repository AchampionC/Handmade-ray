#pragma once
#include "raymath.h"
typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;
typedef float f32;

#define F32MAX FLT_MAX;
#define F32MIN FLT_MIN; 
#define Pi32 3.15159265359f
#define Tau32 6.28318530717958647692f

#define internal static


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