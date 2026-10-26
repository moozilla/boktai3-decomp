#include "global.h"
#include "render.h"

// MATCHING_CONCESSION: register-pinning

struct PaletteBank16 {
    u16 colors[16];
};
void CpuFastSet(const void *src, void *dst, u32 control);

static inline u32 isKey32E(u32 key)
{
    if (key == 0x32E)
        return 1;
    return 0;
}

#define BLEND_LOOP()                                                                               \
    for (j = 0; j < 16; j++) {                                                                     \
        u32 c = *src;                                                                              \
        *dst = (((((u16)c & 0x7C1F) * level + rb * inverse) & 0xF83E0) |                           \
                (((c & 0x3E0) * level + g * inverse) & 0x7C00)) >>                                 \
               5;                                                                                  \
        src++;                                                                                     \
        dst++;                                                                                     \
    }

#define SKIP_TESTS(i)                                                                              \
    {                                                                                              \
        u32 flags = gObjFadeFlags;                                                                 \
        if (flags & 1) {                                                                           \
            u32 key = gObjPaletteKeys[i];                                                          \
            if (key == 0x2A || key == 0x33 || key == 0x34 || key == 0x157 || key == 0 ||           \
                key == 0x3A9 || key == 0x35)                                                       \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 2) {                                                                           \
            u32 key = gObjPaletteKeys[i];                                                          \
            if (key == 0x3AA || key == 0x23A || key == 0x3AA || key == 0x2A || key == 0x33 ||      \
                key == 0x34)                                                                       \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 0x10) {                                                                        \
            u32 key = gObjPaletteKeys[i];                                                          \
            if (key == 0x2A || key == 0x33 || key == 0x34 || key - 0x283 <= 8)                     \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 0x20) {                                                                        \
            if (isKey32E(gObjPaletteKeys[i]))                                                      \
                continue;                                                                          \
        }                                                                                          \
        if (gObjFadeFlags & 0x40) {                                                                \
            u32 key = gObjPaletteKeys[i];                                                          \
            if (key - 0x3AA <= 5 || key - 0x1F7 <= 3)                                              \
                continue;                                                                          \
        }                                                                                          \
    }

void sub_08217430(void)
{
    register s32 level;
    s32 inverse;
    s32 i;
    if (((*(s32 *)&gObjFadeLevel) == 0x40) && ((*(s32 *)&gObjPaletteEffect) == 0)) {
        if ((*(s32 *)&gObjFadeApplied) != 0x40) {
            for (i = 0; i < (*(s32 *)&gPaletteBanksInUse); i++) {
                u8 **basePtr = &gPaletteBankData;
                u32 offset = gObjPaletteKeys[i] << 5;
                u8 *base = *basePtr;
                register u8 *bank asm("r3") = base + offset;
                asm("" : : "r"(offset), "r"(base));
                CpuFastSet(bank, &((struct PaletteBank16 *)gObjPaletteBuffer)[i], 8);
            }
        }
        (*(s32 *)&gObjFadeApplied) = (*(s32 *)&gObjFadeLevel);
        return;
    }
    level = (*(s32 *)&gObjFadeLevel) >> 1;
    inverse = 32 - level;
    if (gObjFadeColor == 0) {
        for (i = 0; i < 16; i++) {
            u16 *src;
            u16 *dst;
            s32 j;
            {
                if (gObjFadeFlags & 1) {
                    u32 key = gObjPaletteKeys[i];
                    if (((((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) || (key == 0x157)) ||
                          (key == 0)) ||
                         (key == 0x3A9)) ||
                        (key == 0x35))
                        continue;
                }
                if (gObjFadeFlags & 2) {
                    u32 key = gObjPaletteKeys[i];
                    if ((((((key == 0x3AA) || (key == 0x23A)) || (key == 0x3AA)) ||
                          (key == 0x2A)) ||
                         (key == 0x33)) ||
                        (key == 0x34))
                        continue;
                }
                if (gObjFadeFlags & 0x10) {
                    u32 key = gObjPaletteKeys[i];
                    if ((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) || ((key - 0x283) <= 8))
                        continue;
                }
                if (gObjFadeFlags & 0x20) {
                    if (isKey32E(gObjPaletteKeys[i]))
                        continue;
                }
                if (gObjFadeFlags & 0x40) {
                    u32 key = gObjPaletteKeys[i];
                    if (((key - 0x3AA) <= 5) || ((key - 0x1F7) <= 3))
                        continue;
                }
            };
            if (i < (*(s32 *)&gPaletteBanksInUse))
                src = (u16 *)(gPaletteBankData + (gObjPaletteKeys[i] << 5));
            else
                src = ((struct PaletteBank16 *)gObjPaletteBuffer)[i].colors;
            dst = ((struct PaletteBank16 *)gObjPaletteBuffer)[i].colors;
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
        if ((*(s32 *)&gObjPaletteEffect) == 1) {
            s32 n;
            for (n = 0; n < 16; n++) {
                u32 keep = 1;
                u32 key = gObjPaletteKeys[n];
                u16 *src;
                u16 *dst;
                s32 j;
                if (((((((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) || (key == 0x157)) ||
                        (key == 0)) ||
                       (key == 0x3A9)) ||
                      (key == 0x35)) ||
                     ((key - 0x3AA) <= 5)) ||
                    ((key - 0x1F7) <= 3))
                    keep = 0;
                if (keep) {
                    dst = ((struct PaletteBank16 *)gObjPaletteBuffer)[n].colors;
                    for (j = 0; j < 16; j++)
                        *(dst++) = gBgFadeColor;

                } else {
                    if (n < (*(s32 *)&gPaletteBanksInUse))
                        src = (u16 *)(gPaletteBankData + (gObjPaletteKeys[n] << 5));
                    else
                        src = ((struct PaletteBank16 *)gObjPaletteBuffer)[n].colors;
                    dst = ((struct PaletteBank16 *)gObjPaletteBuffer)[n].colors;
                    for (j = 0; j < 16; j++) {
                        u32 c = *src;
                        *dst = (((((((u16)c) & 0x7C1F) * level) + (rb * inverse)) & 0xF83E0) |
                                ((((c & 0x3E0) * level) + (g * inverse)) & 0x7C00)) >>
                               5;
                        src++;
                        dst++;
                    }

                    ;
                }
            }

        } else {
            s32 i;
            for (i = 0; i < 16; i++) {
                u16 *src;
                u16 *dst;
                s32 j;
                {
                    if (gObjFadeFlags & 1) {
                        u32 key = gObjPaletteKeys[i];
                        if (((((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) ||
                               (key == 0x157)) ||
                              (key == 0)) ||
                             (key == 0x3A9)) ||
                            (key == 0x35))
                            continue;
                    }
                    if (gObjFadeFlags & 2) {
                        u32 key = gObjPaletteKeys[i];
                        if ((((((key == 0x3AA) || (key == 0x23A)) || (key == 0x3AA)) ||
                              (key == 0x2A)) ||
                             (key == 0x33)) ||
                            (key == 0x34))
                            continue;
                    }
                    if (gObjFadeFlags & 0x10) {
                        u32 key = gObjPaletteKeys[i];
                        if ((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) ||
                            ((key - 0x283) <= 8))
                            continue;
                    }
                    if (gObjFadeFlags & 0x20) {
                        if (isKey32E(gObjPaletteKeys[i]))
                            continue;
                    }
                    if (gObjFadeFlags & 0x40) {
                        u32 key = gObjPaletteKeys[i];
                        if (((key - 0x3AA) <= 5) || ((key - 0x1F7) <= 3))
                            continue;
                    }
                };
                if (i < (*(s32 *)&gPaletteBanksInUse))
                    src = (u16 *)(gPaletteBankData + (gObjPaletteKeys[i] << 5));
                else
                    src = ((struct PaletteBank16 *)gObjPaletteBuffer)[i].colors;
                dst = ((struct PaletteBank16 *)gObjPaletteBuffer)[i].colors;
                for (j = 0; j < 16; j++) {
                    u32 c = *src;
                    *dst = (((((((u16)c) & 0x7C1F) * level) + (rb * inverse)) & 0xF83E0) |
                            ((((c & 0x3E0) * level) + (g * inverse)) & 0x7C00)) >>
                           5;
                    src++;
                    dst++;
                }

                ;
            }
        }
    }
    (*(s32 *)&gObjFadeApplied) = (*(s32 *)&gObjFadeLevel);
}
