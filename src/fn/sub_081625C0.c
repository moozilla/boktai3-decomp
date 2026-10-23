#include "global.h"
#define PlayerAttackEffect_Initialize sub_081625C0
void AnimData_Load(u8 *, u32);
void AnimData_SelectPaletteBank(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
void *ScriptProc_Start(u32, u32);
s32 PlayerAttackEffect_Initialize(u8 *s, u8 *source, void *a, void *b)
{
    u8 *p = s + 0x18;
    u8 *entry;
    s32 i;
    u32 zero;
    struct Pair {
        u32 x, y;
    };
    struct Flags {
        u32 field;
        u8 pad[12];
    };
    struct State {
        u8 gap[0x484];
        struct Flags slots[16];
    };
    u32 key;
    AnimData_Load(p, 0x7E5F);
    p += 0x1C;
    key = 0x6250;
    AnimData_Load(p, key);
    AnimData_SelectPaletteBank(p, 0x116);
    p += 0x1C;
    AnimData_Load(p, key);
    AnimData_SelectPaletteBank(p, 0x118);
    p += 0x1C;
    AnimData_Load(p, key);
    AnimData_SelectPaletteBank(p, 0x119);
    p += 0x1C;
    AnimData_Load(p, key);
    AnimData_SelectPaletteBank(p, 0x11A);
    p += 0x1C;
    AnimData_Load(p, key);
    AnimData_SelectPaletteBank(p, 0x11B);
    entry = s + 0xC4;
    i = 0;
    zero = 0;
    do {
        Sprite_Init(entry, p, 1);
        *(u16 *)(entry + 0x10) = zero;
        *(struct Pair *)(entry + 0x1C) = *(struct Pair *)(source + 0x20);
        entry[7] = 1;
        ((struct State *)s)->slots[i].field = zero;
        entry += 0x2C;
        i++;
    } while (i <= 15);
    *(void **)(s + 0x5A4) = ScriptProc_Start(0x922E, 0x981D);
    *(u8 **)(s + 0x594) = source;
    *(void **)(s + 0x58C) = a;
    *(void **)(s + 0x590) = b;
    *(u32 *)(s + 0x584) = 0;
    return 0;
}