#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: records attack contacts and queues a timed reaction.
#define Mummy_OnAttackCollision sub_080D0C30
// Pointer lifetimes and response field addressing still differ.
static inline u8 blocked2048(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    typeof(mask) c1;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    c1 = mask;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & c1)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;

zero:
    return 0;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

void *SceneActorContacts_FindObjectById(u16);
static inline u32 WordFlag(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 SaveFlag(u8 *state, u16 **saved, u32 mask)
{
    u16 *p = (void *)(state + 0x6F2);
    u32 v = *p & mask;
    *saved = p;
    if (v)
        return 1;
    return 0;
}
// MATCHING_CONCESSION: register-pinning
static inline u32 Arg1(u32 k_, u8 *base)
{
    return base + k_;
}

void Mummy_OnAttackCollision(u8 *unused, u8 *obj, u8 *input)
{
    u8 *base = input;
    u8 *state = *((u8 **)(input + 0x3D0));
    typeof(*((volatile u16 *)(base + 0x172))) t1;
    if (blocked2048(input))
        return;
    if (((u16)((*((u16 *)(obj + 10))) - 1)) <= 7) {
        u16 *flags;
        if ((!SaveFlag(state, &flags, 0x800)) &&
            WordFlag((u32 *)((*((u8 **)(obj + 0x50))) + 0x20), 8))
            return;
        {
            u32 mask = ~0x800;
            *flags &= mask;
        }
        if ((!blocked2048(input)) && ((*((u16 *)(obj + 10))) & 15)) {
            *((u32 *)(input + 0x304)) = *((u16 *)(obj + 4));
            *((u32 *)(input + 0x2D0)) &= 0xFEFFFFFF;
        }
        *((void **)(base + 0xDC)) = SceneActorContacts_FindObjectById(*((u16 *)(base + 0x304)));
        obj[0x46] |= 1;
        if ((!(*((u16 *)Arg1(0x178, base)))) &&
            WordFlag((u32 *)((*((u8 **)(obj + 0x50))) + 0x20), 0x200)) {
            *((u32 *)(base + 0x2E0)) = 1;
            *((u32 *)(base + 0x2DC)) = 4;
            *((u32 *)(base + 0x2D8)) = 0;
            {
                u16 *d1 = (u16 *)(base + 0x170);
                d1[1] = 1;
            }
            t1 = *((volatile u16 *)(base + 0x172));
            *((u16 *)(base + 0x2E4)) = t1;
            *((u16 *)(base + 0x174)) = 1;
            *((u16 *)(base + 0x178)) = 30;
            *((u16 *)(base + 0x2E6)) = 4;
        }
    } else if ((!blocked2048(input)) && ((*((u16 *)(obj + 10))) & 15)) {
        *((u32 *)(input + 0x304)) = *((u16 *)(obj + 4));
        *((u32 *)(input + 0x2D0)) &= 0xFEFFFFFF;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080D0C30);
#endif
