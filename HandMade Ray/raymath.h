#pragma once

#include <math.h>
#ifndef raymath
#define raymath

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef s32 b32;
typedef s32 b32x;

typedef float f32;

union v2
{
	struct
	{
		float x, y;
	};
	struct
	{
		float u, v;
	};
	float E[2];
};

union v3
{
	struct
	{
		float x, y, z;
	};
	struct
	{
		float u, v, __;
	};
	struct
	{
		float r, g, b;
	};
	struct
	{
		v2 xy;
		float Ignored0_;
	};
	struct
	{
		float Ignored1_;
		v2 yz;
	};
	struct
	{
		v2 uv;
		float Ignored2_;
	};
	struct
	{
		float Ignored3_;
		v2 v__;
	};
	float E[3];
};

union v4
{
	struct
	{
		union
		{
			v3 xyz;
			struct
			{
				float x, y, z;
			};
		};

		float w;
	};
	struct
	{
		union
		{
			v3 rgb;
			struct
			{
				float r, g, b;
			};
		};

		float a;
	};
	struct
	{
		v2 xy;
		float Ignored0_;
		float Ignored1_;
	};
	struct
	{
		float Ignored2_;
		v2 yz;
		float Ignored3_;
	};
	struct
	{
		float Ignored4_;
		float Ignored5_;
		v2 zw;
	};
	float E[4];
};


inline float
SquareRoot(float A)
{
	float Result = (float)sqrt(A);
	return(Result);
}

inline float
Pow(float A, float B)
{
	float Result = (float)pow(A, B);
	return(Result);
}

inline u32 RoundReal32ToUInt32(f32 F)
{
	u32 Result = (u32)(F + 0.5f);
	return(Result);
}

inline v2
V2i(s32 X, s32 Y)
{
	v2 Result = { (float)X, (float)Y };

	return(Result);
}

inline v2
V2i(u32 X, u32 Y)
{
	v2 Result = { (float)X, (float)Y };

	return(Result);
}

inline v2
V2(float X, float Y)
{
	v2 Result;

	Result.x = X;
	Result.y = Y;

	return(Result);
}

inline v3
V3(float X, float Y, float Z)
{
	v3 Result;

	Result.x = X;
	Result.y = Y;
	Result.z = Z;

	return(Result);
}

inline v3
V3(v2 XY, float Z)
{
	v3 Result;

	Result.x = XY.x;
	Result.y = XY.y;
	Result.z = Z;

	return(Result);
}

inline v4
V4(float X, float Y, float Z, float W)
{
	v4 Result;

	Result.x = X;
	Result.y = Y;
	Result.z = Z;
	Result.w = W;

	return(Result);
}

inline v4
V4(v3 XYZ, float W)
{
	v4 Result;

	Result.xyz = XYZ;
	Result.w = W;

	return(Result);
}

inline float
Square(float A)
{
	float Result = A * A;

	return(Result);
}

inline float
Triangle01(float t)
{
	float Result = 2.0f * t;
	if (Result > 1.0f)
	{
		Result = 2.0f - Result;
	}

	return(Result);
}

inline float
Lerp(float A, float t, float B)
{
	float Result = (1.0f - t) * A + t * B;

	return(Result);
}

inline s32
Clamp(s32 Min, s32 Value, s32 Max)
{
	s32 Result = Value;

	if (Result < Min)
	{
		Result = Min;
	}
	else if (Result > Max)
	{
		Result = Max;
	}

	return(Result);
}

inline float
Clamp(float Min, float Value, float Max)
{
	float Result = Value;

	if (Result < Min)
	{
		Result = Min;
	}
	else if (Result > Max)
	{
		Result = Max;
	}

	return(Result);
}

inline float
Clamp01(float Value)
{
	float Result = Clamp(0.0f, Value, 1.0f);

	return(Result);
}

inline float
Clamp01MapToRange(float Min, float t, float Max)
{
	float Result = 0.0f;

	float Range = Max - Min;
	if (Range != 0.0f)
	{
		Result = Clamp01((t - Min) / Range);
	}

	return(Result);
}

inline float
ClampAboveZero(float Value)
{
	float Result = (Value < 0) ? 0.0f : Value;
	return(Result);
}

inline float
SafeRatioN(float Numerator, float Divisor, float N)
{
	float Result = N;

	if (Divisor != 0.0f)
	{
		Result = Numerator / Divisor;
	}

	return(Result);
}

inline float
SafeRatio0(float Numerator, float Divisor)
{
	float Result = SafeRatioN(Numerator, Divisor, 0.0f);

	return(Result);
}

inline float
SafeRatio1(float Numerator, float Divisor)
{
	float Result = SafeRatioN(Numerator, Divisor, 1.0f);

	return(Result);
}


inline v2
Perp(v2 A)
{
	v2 Result = { -A.y, A.x };
	return(Result);
}

inline v2
operator*(float A, v2 B)
{
	v2 Result;

	Result.x = A * B.x;
	Result.y = A * B.y;

	return(Result);
}

inline v2
operator*(v2 B, float A)
{
	v2 Result = A * B;

	return(Result);
}

inline v2&
operator*=(v2& B, float A)
{
	B = A * B;

	return(B);
}

inline v2
operator-(v2 A)
{
	v2 Result;

	Result.x = -A.x;
	Result.y = -A.y;

	return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
	v2 Result;

	Result.x = A.x + B.x;
	Result.y = A.y + B.y;

	return(Result);
}

inline v2&
operator+=(v2& A, v2 B)
{
	A = A + B;

	return(A);
}

inline v2
operator-(v2 A, v2 B)
{
	v2 Result;

	Result.x = A.x - B.x;
	Result.y = A.y - B.y;

	return(Result);
}

inline v2&
operator-=(v2& A, v2 B)
{
	A = A - B;

	return(A);
}

inline v2
Hadamard(v2 A, v2 B)
{
	v2 Result = { A.x * B.x, A.y * B.y };

	return(Result);
}

inline float
Inner(v2 A, v2 B)
{
	float Result = A.x * B.x + A.y * B.y;

	return(Result);
}

inline float
LengthSq(v2 A)
{
	float Result = Inner(A, A);

	return(Result);
}

inline float
Length(v2 A)
{
	float Result = SquareRoot(LengthSq(A));
	return(Result);
}

inline v2
Clamp01(v2 Value)
{
	v2 Result;

	Result.x = Clamp01(Value.x);
	Result.y = Clamp01(Value.y);

	return(Result);
}

inline v3
operator*(float A, v3 B)
{
	v3 Result;

	Result.x = A * B.x;
	Result.y = A * B.y;
	Result.z = A * B.z;

	return(Result);
}

inline v3
operator*(v3 B, float A)
{
	v3 Result = A * B;

	return(Result);
}

inline v3&
operator*=(v3& B, float A)
{
	B = A * B;

	return(B);
}

inline v3
operator/(v3 B, float A)
{
	v3 Result = (1.0f / A) * B;

	return(Result);
}

inline v3&
operator/=(v3& B, float A)
{
	B = B / A;

	return(B);
}

inline v3
operator-(v3 A)
{
	v3 Result;

	Result.x = -A.x;
	Result.y = -A.y;
	Result.z = -A.z;

	return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
	v3 Result;

	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	Result.z = A.z + B.z;

	return(Result);
}

inline v3&
operator+=(v3& A, v3 B)
{
	A = A + B;

	return(A);
}

inline v3
operator-(v3 A, v3 B)
{
	v3 Result;

	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	Result.z = A.z - B.z;

	return(Result);
}

inline v3&
operator-=(v3& A, v3 B)
{
	A = A - B;

	return(A);
}

inline v3
Hadamard(v3 A, v3 B)
{
	v3 Result = { A.x * B.x, A.y * B.y, A.z * B.z };

	return(Result);
}

inline float
Inner(v3 A, v3 B)
{
	float Result = A.x * B.x + A.y * B.y + A.z * B.z;

	return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
	v3 Result;

	Result.x = A.y * B.z - A.z * B.y;
	Result.y = A.z * B.x - A.x * B.z;
	Result.z = A.x * B.y - A.y * B.x;

	return(Result);
}

inline float
LengthSq(v3 A)
{
	float Result = Inner(A, A);

	return(Result);
}

inline float
Length(v3 A)
{
	float Result = SquareRoot(LengthSq(A));
	return(Result);
}

inline v3
Normalize(v3 A)
{
	v3 Result = A * (1.0f / Length(A));

	return(Result);
}

inline v3
NOZ(v3 A)
{
	v3 Result = {};

	float LenSq = LengthSq(A);
	if (LenSq > Square(0.0001f))
	{
		Result = A * (1.0f / SquareRoot(LenSq));
	}

	return(Result);
}

inline v3
Clamp01(v3 Value)
{
	v3 Result;

	Result.x = Clamp01(Value.x);
	Result.y = Clamp01(Value.y);
	Result.z = Clamp01(Value.z);

	return(Result);
}

inline v3
Lerp(v3 A, float t, v3 B)
{
	v3 Result = (1.0f - t) * A + t * B;

	return(Result);
}


inline v4
operator*(float A, v4 B)
{
	v4 Result;

	Result.x = A * B.x;
	Result.y = A * B.y;
	Result.z = A * B.z;
	Result.w = A * B.w;

	return(Result);
}

inline v4
operator*(v4 B, float A)
{
	v4 Result = A * B;

	return(Result);
}

inline v4&
operator*=(v4& B, float A)
{
	B = A * B;

	return(B);
}

inline v4
operator-(v4 A)
{
	v4 Result;

	Result.x = -A.x;
	Result.y = -A.y;
	Result.z = -A.z;
	Result.w = -A.w;

	return(Result);
}

inline v4
operator+(v4 A, v4 B)
{
	v4 Result;

	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	Result.z = A.z + B.z;
	Result.w = A.w + B.w;

	return(Result);
}

inline v4&
operator+=(v4& A, v4 B)
{
	A = A + B;

	return(A);
}

inline v4
operator-(v4 A, v4 B)
{
	v4 Result;

	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	Result.z = A.z - B.z;
	Result.w = A.w - B.w;

	return(Result);
}

inline v4&
operator-=(v4& A, v4 B)
{
	A = A - B;

	return(A);
}

inline v4
Hadamard(v4 A, v4 B)
{
	v4 Result = { A.x * B.x, A.y * B.y, A.z * B.z, A.w * B.w };

	return(Result);
}

inline float
Inner(v4 A, v4 B)
{
	float Result = A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;

	return(Result);
}

inline float
LengthSq(v4 A)
{
	float Result = Inner(A, A);

	return(Result);
}

inline float
Length(v4 A)
{
	float Result = SquareRoot(LengthSq(A));
	return(Result);
}

inline v4
Clamp01(v4 Value)
{
	v4 Result;

	Result.x = Clamp01(Value.x);
	Result.y = Clamp01(Value.y);
	Result.z = Clamp01(Value.z);
	Result.w = Clamp01(Value.w);

	return(Result);
}

inline v4
Lerp(v4 A, float t, v4 B)
{
	v4 Result = (1.0f - t) * A + t * B;

	return(Result);
}

inline v4
sRGBLinearize(v4 C)
{
	v4 Result;

	Result.r = Square(C.r);
	Result.g = Square(C.g);
	Result.b = Square(C.b);
	Result.a = C.a;

	return(Result);
}

inline v4
LinearTosRGB(v4 C)
{
	v4 Result;

	Result.r = SquareRoot(C.r);
	Result.g = SquareRoot(C.g);
	Result.b = SquareRoot(C.b);
	Result.a = C.a;

	return(Result);
}

inline v4
sRGBLinearize(float R, float G, float B, float A)
{
	v4 Input = { R, G, B, A };
	v4 Result = sRGBLinearize(Input);
	return(Result);
}

inline v4
SRGB255ToLinear1(v4 C)
{
	v4 Result;

	float Inv255 = 1.0f / 255.0f;

	Result.r = Square(Inv255 * C.r);
	Result.g = Square(Inv255 * C.g);
	Result.b = Square(Inv255 * C.b);
	Result.a = Inv255 * C.a;

	return(Result);
}

inline v4
Linear1ToSRGB255(v4 C)
{
	v4 Result;

	float One255 = 255.0f;

	Result.r = One255 * SquareRoot(C.r);
	Result.g = One255 * SquareRoot(C.g);
	Result.b = One255 * SquareRoot(C.b);
	Result.a = One255 * C.a;

	return(Result);
}

static v2
RayIntersect2(v2 Pa, v2 ra, v2 Pb, v2 rb)
{
	v2 Result = {};

	float d = (rb.x * ra.y - rb.y * ra.x);
	if (d != 0.0f)
	{
		float ta = ((Pa.x - Pb.x) * rb.y + (Pb.y - Pa.y) * rb.x) / d;
		float tb = ((Pa.x - Pb.x) * ra.y + (Pb.y - Pa.y) * ra.x) / d;

		Result = V2(ta, tb);
	}

	return(Result);
}

inline v4
BGRAUnpack4x8(u32 Packed)
{
	v4 Result = { (float)((Packed >> 16) & 0xFF),
				 (float)((Packed >> 8) & 0xFF),
				 (float)((Packed >> 0) & 0xFF),
				 (float)((Packed >> 24) & 0xFF) };

	return(Result);
}

inline u32
BGRAPack4x8(v4 Unpacked)
{
	u32 Result = ((RoundReal32ToUInt32(Unpacked.a) << 24) |
		(RoundReal32ToUInt32(Unpacked.r) << 16) |
		(RoundReal32ToUInt32(Unpacked.g) << 8) |
		(RoundReal32ToUInt32(Unpacked.b) << 0));

	return(Result);
}

inline v4
RGBAUnpack4x8(u32 Packed)
{
	v4 Result = { (float)((Packed >> 0) & 0xFF),
				 (float)((Packed >> 8) & 0xFF),
				 (float)((Packed >> 16) & 0xFF),
				 (float)((Packed >> 24) & 0xFF) };

	return(Result);
}

inline u32
RGBAPack4x8(v4 Unpacked)
{
	u32 Result = ((RoundReal32ToUInt32(Unpacked.a) << 24) |
		(RoundReal32ToUInt32(Unpacked.b) << 16) |
		(RoundReal32ToUInt32(Unpacked.g) << 8) |
		(RoundReal32ToUInt32(Unpacked.r) << 0));

	return(Result);
}

inline s32 IsInRange(float Min, float Value, float Max)
{
	s32 Result = ((Min <= Value) &&
		(Value <= Max));

	return(Result);
}


#endif