#include "global.h"
void AnimData_Load(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
void *ScriptProc_Start(u32, u32);
void sub_081A6FF4(u8 *, u8 *);
void sub_081A6F34(u8 *);
void sub_081A6EBC(void);
void sub_0815F6F0(u8 *, u8 *, void (*)(void));
void sub_081A7040(u8 *s, u8 *t)
{
    u8 *obj = t + 0x2C;
    u8 *coords;
    u32 width, height, zero;
    AnimData_Load(obj, 0x89CA);
    Sprite_Init(t, obj, 0);
    *(u16 *)(t + 0x10) = 7;
    t[7] = 3;
    *(u16 *)(obj + 6) = 0x72;
    *(u8 **)(obj + 0xC) = s + 0x778;
    *(void **)(t + 0x58) = ScriptProc_Start(0x922E, 0x572B);
    coords = t + 0x1C;
    width = 0x33B;
    height = 0x100;
    zero = 0;
    *(u16 *)(t + 0x1C) = width;
    *(u16 *)(coords + 2) = height;
    *(u16 *)(coords + 4) = width;
    sub_081A6FF4(s, coords + 0x44);
    sub_081A6F34(t);
    sub_0815F6F0(t + 0x110, t + 0xA8, sub_081A6EBC);
    *(u16 *)(t + 0x120) = zero;
    *(u32 *)(t + height) = zero;
    *(u16 *)(t + 0x106) = zero;
    *(u8 **)(t + 0x130) = s + 0x598;
    *(u8 **)(t + 0x134) = (s += 0xF34);
}
