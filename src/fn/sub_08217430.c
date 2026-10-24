#include "global.h"

// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Left: fast-path bank address register (ROM r3 plus a copy), the flags & 0x20
// ands operand order, and src/dst allocation in the first blend loop.

struct PaletteBank16 {
    u16 colors[16];
};
extern s32 gUnk_030051D4, gUnk_030051C8, gUnk_03004DAC, gUnk_030051CC;
extern u8 *gUnk_030042E4;
extern u16 gUnk_030051F0[];
extern struct PaletteBank16 gUnk_03004DC0[];
extern u16 gUnk_030051DC, gUnk_03005210, gUnk_03005214;
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
        u32 flags = gUnk_03005210;                                                                 \
        if (flags & 1) {                                                                           \
            u32 key = gUnk_030051F0[i];                                                            \
            if (key == 0x2A || key == 0x33 || key == 0x34 || key == 0x157 || key == 0 ||           \
                key == 0x3A9 || key == 0x35)                                                       \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 2) {                                                                           \
            u32 key = gUnk_030051F0[i];                                                            \
            if (key == 0x3AA || key == 0x23A || key == 0x3AA || key == 0x2A || key == 0x33 ||      \
                key == 0x34)                                                                       \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 0x10) {                                                                        \
            u32 key = gUnk_030051F0[i];                                                            \
            if (key == 0x2A || key == 0x33 || key == 0x34 || key - 0x283 <= 8)                     \
                continue;                                                                          \
        }                                                                                          \
        if (flags & 0x20) {                                                                        \
            if (isKey32E(gUnk_030051F0[i]))                                                        \
                continue;                                                                          \
        }                                                                                          \
        if (gUnk_03005210 & 0x40) {                                                                \
            u32 key = gUnk_030051F0[i];                                                            \
            if (key - 0x3AA <= 5 || key - 0x1F7 <= 3)                                              \
                continue;                                                                          \
        }                                                                                          \
    }

void sub_08217430(void)
{
    register s32 level asm("sl");
    s32 inverse;
    s32 i;
    if ((gUnk_030051D4 == 0x40) && (gUnk_030051C8 == 0)) {
        if (gUnk_03004DAC != 0x40) {
            for (i = 0; i < gUnk_030051CC; i++) {
                u8 *bank = gUnk_030042E4 + (gUnk_030051F0[i] << 5);
                CpuFastSet(bank, &gUnk_03004DC0[i], 8);
            }
        }
        gUnk_03004DAC = gUnk_030051D4;
        return;
    }
    level = gUnk_030051D4 >> 1;
    inverse = 32 - level;
    if (gUnk_030051DC == 0) {
        for (i = 0; i < 16; i++) {
            u16 *src;
            u16 *dst;
            s32 j;
            {
                if (gUnk_03005210 & 1) {
                    u32 key = gUnk_030051F0[i];
                    if (((((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) || (key == 0x157)) ||
                          (key == 0)) ||
                         (key == 0x3A9)) ||
                        (key == 0x35))
                        continue;
                }
                if (gUnk_03005210 & 2) {
                    u32 key = gUnk_030051F0[i];
                    if ((((((key == 0x3AA) || (key == 0x23A)) || (key == 0x3AA)) ||
                          (key == 0x2A)) ||
                         (key == 0x33)) ||
                        (key == 0x34))
                        continue;
                }
                if (gUnk_03005210 & 0x10) {
                    u32 key = gUnk_030051F0[i];
                    if ((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) || ((key - 0x283) <= 8))
                        continue;
                }
                if (gUnk_03005210 & 0x20) {
                    if (isKey32E(gUnk_030051F0[i]))
                        continue;
                }
                if (gUnk_03005210 & 0x40) {
                    u32 key = gUnk_030051F0[i];
                    if (((key - 0x3AA) <= 5) || ((key - 0x1F7) <= 3))
                        continue;
                }
            };
            if (i < gUnk_030051CC)
                src = (u16 *)(gUnk_030042E4 + (gUnk_030051F0[i] << 5));
            else
                src = gUnk_03004DC0[i].colors;
            dst = gUnk_03004DC0[i].colors;
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
        u32 rb = gUnk_030051DC & 0x7C1F;
        u32 g = gUnk_030051DC & 0x3E0;
        if (gUnk_030051C8 == 1) {
            s32 n;
            for (n = 0; n < 16; n++) {
                u32 keep = 1;
                u32 key = gUnk_030051F0[n];
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
                    dst = gUnk_03004DC0[n].colors;
                    for (j = 0; j < 16; j++)
                        *(dst++) = gUnk_03005214;

                } else {
                    if (n < gUnk_030051CC)
                        src = (u16 *)(gUnk_030042E4 + (gUnk_030051F0[n] << 5));
                    else
                        src = gUnk_03004DC0[n].colors;
                    dst = gUnk_03004DC0[n].colors;
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
                    if (gUnk_03005210 & 1) {
                        u32 key = gUnk_030051F0[i];
                        if (((((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) ||
                               (key == 0x157)) ||
                              (key == 0)) ||
                             (key == 0x3A9)) ||
                            (key == 0x35))
                            continue;
                    }
                    if (gUnk_03005210 & 2) {
                        u32 key = gUnk_030051F0[i];
                        if ((((((key == 0x3AA) || (key == 0x23A)) || (key == 0x3AA)) ||
                              (key == 0x2A)) ||
                             (key == 0x33)) ||
                            (key == 0x34))
                            continue;
                    }
                    if (gUnk_03005210 & 0x10) {
                        u32 key = gUnk_030051F0[i];
                        if ((((key == 0x2A) || (key == 0x33)) || (key == 0x34)) ||
                            ((key - 0x283) <= 8))
                            continue;
                    }
                    if (gUnk_03005210 & 0x20) {
                        if (isKey32E(gUnk_030051F0[i]))
                            continue;
                    }
                    if (gUnk_03005210 & 0x40) {
                        u32 key = gUnk_030051F0[i];
                        if (((key - 0x3AA) <= 5) || ((key - 0x1F7) <= 3))
                            continue;
                    }
                };
                if (i < gUnk_030051CC)
                    src = (u16 *)(gUnk_030042E4 + (gUnk_030051F0[i] << 5));
                else
                    src = gUnk_03004DC0[i].colors;
                dst = gUnk_03004DC0[i].colors;
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
    gUnk_03004DAC = gUnk_030051D4;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08217430);
#endif
