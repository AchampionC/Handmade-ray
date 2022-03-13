#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;
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
internal void WriteImage(image_u32 Image, const char* OutputFileName)
{
	u32 OutputPixelSize = GetTotalPixelSize(Image);

	bitmap_header Header = {};
	Header.FileType = 0x4D42; // πÃ”–∏Ò Ω
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
	printf("hello world\n");

	u32 OutputWidth = 1280;
	u32 OutputHeight = 720;
	image_u32 Image = AllocateImage(1280, 720);

	u32* Out = Image.Pixels;
	for (u32 Y = 0; Y < Image.Height; ++ Y)
		for (u32 X = 0; X < Image.Width; ++X)
		{
			*Out ++ = (Y < 32) ? 0xFFFF0000 : 0xFF0000FF;
		}

	WriteImage(Image, "test.bmp");
	return 0;
}