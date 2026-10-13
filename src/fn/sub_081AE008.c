#include "global.h"
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(u8 *, void *);
void sub_0821983C(u8 *, u8 *, u32, u32, u32, u32, u32, u8 *);
void Anim_SetKeyframes(u8 *, u32, u32, u32, u32);
void sub_081ADEAC(u8 *, u8 *, u8 *);
void sub_081ADDF0(u8 *, u8 *, u8 *);
void sub_081ADBF8(void);
void SceneActors_SetDamageCallback(u8 *, u8 *, void (*)(void), u32);
void sub_081AE008(u8 *s, u8 *t)
{
    u8 *coords = t + 0x80;
    u32 width = 0x280, height = 0x300;
    u32 zero = 0;
    u8 *obj;
    void *p;
    *(u16 *)coords = width;
    *(u16 *)(coords + 2) = height;
    *(u16 *)(coords + 4) = width;
    p = ScriptProc_Start(0xCB05, 0xB3DE);
    obj = t + 0x60;
    ResourceHeader_Relocate(obj, p);
    sub_0821983C(t, obj, 0, 0, 2, 2, 0x3C, coords);
    Anim_SetKeyframes(t, obj, 11, 1, zero);
    *(u16 *)(t + 0x3A) = 0x6F;
    *(u8 **)(t + 0x48) = s + 0x718;
    sub_081ADEAC(s, t + 0x90, coords);
    t += 0xA8;
    sub_081ADDF0(s, t, coords);
    SceneActors_SetDamageCallback(s, s + 0x6C4, sub_081ADBF8, *(u16 *)(s + 0x590));
}
