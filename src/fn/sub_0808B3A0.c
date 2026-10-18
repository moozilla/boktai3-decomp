typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;
typedef float f32;
typedef double f64;
typedef u8 bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8 vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;
struct BgCnt
{
  u16 priority : 2;
  u16 charBaseBlock : 2;
  u16 dsCharBaseBlock : 2;
  u16 mosaic : 1;
  u16 palettes : 1;
  u16 screenBaseBlock : 5;
  u16 areaOverflowMode : 1;
  u16 screenSize : 2;
};
typedef volatile struct BgCnt vBgCnt;
struct PlttData
{
  u16 r : 5;
  u16 g : 5;
  u16 b : 5;
  u16 unused_15 : 1;
};
struct OamData
{
  u32 y : 8;
  u32 affineMode : 2;
  u32 objMode : 2;
  u32 mosaic : 1;
  u32 bpp : 1;
  u32 shape : 2;
  u32 x : 9;
  u32 matrixNum : 5;
  u32 size : 2;
  u16 tileNum : 10;
  u16 priority : 2;
  u16 paletteNum : 4;
  u16 affineParam;
};
struct BgAffineSrcData
{
  s32 texX;
  s32 texY;
  s16 scrX;
  s16 scrY;
  s16 sx;
  s16 sy;
  u16 alpha;
};
struct BgAffineDstData
{
  s16 pa;
  s16 pb;
  s16 pc;
  s16 pd;
  s32 dx;
  s32 dy;
};
struct ObjAffineSrcData
{
  s16 xScale;
  s16 yScale;
  u16 rotation;
};
struct SioMultiCnt
{
  u16 baudRate : 2;
  u16 si : 1;
  u16 sd : 1;
  u16 id : 2;
  u16 error : 1;
  u16 enable : 1;
  u16 unused_11_8 : 4;
  u16 mode : 2;
  u16 intrEnable : 1;
  u16 unused_15 : 1;
  u16 data;
};
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
void Script_Call(u32, void *);
void sub_0808B3A0(u8 *p)
{
  s32 k2 = 0;
  u8 *q = *((u8 **) (p + 0x3D0));
  u16 *script = (u16 *) (q + 0x624);
  u8 *count;
  u8 *limit;
  if ((*script) != 0)
  {
    count = q + 0x627;
    *count += 1;
    limit = q + 0x626;
    if ((*count) >= (*limit))
    {
      u8 z;
      Script_Call(*script, k2);
      z = 0;
      *script = 0;
      *limit = z;
      *count = z;
    }
  }
}

