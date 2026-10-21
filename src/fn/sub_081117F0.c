#include "global.h"
#include "engine.h"
void PaletteFade_SetupBackground(u8 *, s32, s32, s32, s32);
s32 PaletteFade_LoadPaletteResources(u8 *, s32, s32);
void ScanlineScroll_Update(void);
void BackgroundScroll_UpdateHorizontal(void);

static inline s32 NextArgShifted(void)
{
    if (Script_GetPc())
        return Script_GetValue() << 7;
    return 0;
}

static inline s32 NextArg(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

s32 PaletteFade_InitFromScript(u8 *p)
{
    s32 a, b, c, d;
    u32 mode;
    s32 r;
    if (!Script_SeekToKeyword('b'))
        goto fail;
    a = NextArg();
    b = NextArg();
    c = NextArg();
    d = NextArg();
    PaletteFade_SetupBackground(p, a, b, c, d);
    *(s32 *)(p + 0x1AE0) = Script_GetKeywordValue('f', 2);
    *(u8 *)(p + 0x184C) = Script_GetKeywordValue('p', 0);
    mode = *(u32 *)(p + 0x1AE0);
    if (mode & 1) {
        if (!Script_SeekToKeyword('s'))
            goto fail;
        *(u16 *)(p + 0x1AD8) = NextArg();
        *(u16 *)(p + 0x1ADA) = NextArg();
        if (!Script_SeekToKeyword('l'))
            goto fail;
        *(u16 *)(p + 0x1ADC) = NextArg();
        *(u16 *)(p + 0x1ADE) = NextArg();
        *(void (**)(void))(p + 0x18) = ScanlineScroll_Update;
    } else if (mode & 2) {
        if (!Script_SeekToKeyword('s'))
            goto fail;
        *(u16 *)(p + 0x1844) = NextArgShifted();
        *(u16 *)(p + 0x1846) = NextArgShifted();
        *(void (**)(void))(p + 0x18) = BackgroundScroll_UpdateHorizontal;
    }
    goto done;
fail:
    return -1;
done:
    Memory_Clear(p + 0x1858, 0x140);
    Memory_Clear(p + 0x1998, 0x140);
    r = PaletteFade_LoadPaletteResources(p, c, d);
    *(u8 **)0x020001D8 = p;
    return r;
}
