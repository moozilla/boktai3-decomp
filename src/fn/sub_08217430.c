#include "global.h"
#include "render.h"

// MATCHING_CONCESSION: register-pinning

void CpuFastSet(const void *src, void *dst, u32 control);

static inline u32 ObjectFade_IsKey32E(u32 key)
{
    if (key == 0x32E)
        return 1;
    return 0;
}

static inline u32 ObjectFade_SkipBank(s32 i)
{
    if (gObjFadeFlags & 1) {
        u32 key = gObjPaletteKeys[i];
        if (key == 0x2A || key == 0x33 || key == 0x34 || key == 0x157 || key == 0 || key == 0x3A9 ||
            key == 0x35)
            return 1;
    }
    if (gObjFadeFlags & 2) {
        u32 key = gObjPaletteKeys[i];
        if (key == 0x3AA || key == 0x23A || key == 0x3AA || key == 0x2A || key == 0x33 ||
            key == 0x34)
            return 1;
    }
    if (gObjFadeFlags & 0x10) {
        u32 key = gObjPaletteKeys[i];
        if (key == 0x2A || key == 0x33 || key == 0x34 || key - 0x283 <= 8)
            return 1;
    }
    if (gObjFadeFlags & 0x20) {
        if (ObjectFade_IsKey32E(gObjPaletteKeys[i]))
            return 1;
    }
    if (gObjFadeFlags & 0x40) {
        u32 key = gObjPaletteKeys[i];
        if (key - 0x3AA <= 5 || key - 0x1F7 <= 3)
            return 1;
    }
    return 0;
}
static inline u16 *ObjectFade_SourceBank(s32 i)
{
    if (i < (s32)gPaletteBanksInUse)
        return (u16 *)(gPaletteBankData + (gObjPaletteKeys[i] << 5));
    return gObjPaletteBuffer[i];
}
void sub_08217430(void)
{
    s32 level;
    s32 inverse;
    s32 i;
    if (gObjFadeLevel == 0x40 && gObjPaletteEffect == 0) {
        if (gObjFadeApplied != 0x40) {
            for (i = 0; i < (s32)gPaletteBanksInUse; i++) {
                u8 **basePtr = &gPaletteBankData;
                u32 offset = gObjPaletteKeys[i] << 5;
                u8 *base = *basePtr;
                register u8 *bank asm("r3") = base + offset;
                asm("" : : "r"(offset), "r"(base));
                CpuFastSet(bank, gObjPaletteBuffer[i], 8);
            }
        }
        gObjFadeApplied = gObjFadeLevel;
        return;
    }
    level = (s32)gObjFadeLevel >> 1;
    inverse = 32 - level;
    if (gObjFadeColor == 0) {
        for (i = 0; i < 16; i++) {
            u16 *src;
            u16 *dst;
            s32 j;
            if (ObjectFade_SkipBank(i))
                continue;
            src = ObjectFade_SourceBank(i);
            dst = gObjPaletteBuffer[i];
            for (j = 0; j < 16; j++) {
                u32 c = *src;
                *dst = ((((((u16)c) & 0x7C1F) * level) & 0xF83E0) |
                        (((c & 0x3E0) * level) & 0x7C00)) >>
                       5;
                do {
                    src++;
                } while (0);
                dst++;
            }
        }

    } else {
        u32 rb = gObjFadeColor & 0x7C1F;
        u32 g = gObjFadeColor & 0x3E0;
        if (gObjPaletteEffect == 1) {
            s32 n;
            for (n = 0; n < 16; n++) {
                u32 keep = 1;
                u32 key = gObjPaletteKeys[n];
                u16 *src;
                u16 *dst;
                s32 j;
                if (key == 0x2A || key == 0x33 || key == 0x34 || key == 0x157 || key == 0 ||
                    key == 0x3A9 || key == 0x35 || key - 0x3AA <= 5 || key - 0x1F7 <= 3)
                    keep = 0;
                if (keep) {
                    dst = gObjPaletteBuffer[n];
                    for (j = 0; j < 16; j++)
                        *dst++ = gBgFadeColor;

                } else {
                    src = ObjectFade_SourceBank(n);
                    dst = gObjPaletteBuffer[n];
                    for (j = 0; j < 16; j++) {
                        u32 c = *src;
                        *dst = (((((((u16)c) & 0x7C1F) * level) + (rb * inverse)) & 0xF83E0) |
                                ((((c & 0x3E0) * level) + (g * inverse)) & 0x7C00)) >>
                               5;
                        src++;
                        dst++;
                    }
                }
            }

        } else {
            s32 i;
            for (i = 0; i < 16; i++) {
                u16 *src;
                u16 *dst;
                s32 j;
                if (ObjectFade_SkipBank(i))
                    continue;
                src = ObjectFade_SourceBank(i);
                dst = gObjPaletteBuffer[i];
                for (j = 0; j < 16; j++) {
                    u32 c = *src;
                    *dst = (((((((u16)c) & 0x7C1F) * level) + (rb * inverse)) & 0xF83E0) |
                            ((((c & 0x3E0) * level) + (g * inverse)) & 0x7C00)) >>
                           5;
                    src++;
                    dst++;
                }
            }
        }
    }
    gObjFadeApplied = gObjFadeLevel;
}
