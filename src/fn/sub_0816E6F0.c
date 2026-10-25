#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94.6%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Anim_LoadKeyframe(void *, void *, u32, u32);

static inline s32 Halve(s32 value)
{
    return value >= 0 ? value >> 1 : -(-value >> 1);
}

static inline s32 Shift12(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline void RandomSpin(s16 *spin)
{
    u16 *table = gUnk_0203B400;
    u32 *index = &gUnk_03005308;
    u32 mask;
    u32 i;
    s32 v;

    i = *index + 1;
    mask = 0x3FF;
    i &= mask;
    *index = i;
    v = Mod(table[i], 10) + 5;
    *spin = v;
    i = (*index + 1) & mask;
    *index = i;
    if (table[i] & 1)
        *spin = -v;
}

inline static void RandomSpin_0816E6F0(s16 *spin)
{
    u16 *table = gUnk_0203B400;
    u32 *index = &gUnk_03005308;
    u32 mask;
    u32 i;
    s32 v;
    i = (*index) + 1;
    mask = 0x3FF;
    i &= mask;
    *index = i;
    v = Mod(table[i], 10) + 5;
    *spin = v;
    i = ((*index) + 1) & mask;
    *index = i;
    {
        typeof(table[i]) v1 = table[i];
        if (v1 & 1)
            *spin = -v;
    }
}

s32 sub_0816E6F0(u8 *m)
{
    s16 *pos = (s16 *)(m + 0x43EC);
    s16 *vel = (s16 *)(m + 0x43F4);
    u16 *angle = (u16 *)(m + 0x4406);
    s16 *spin = (s16 *)(m + 0x4404);
    s32 stopped;
    s32 a;
    s32 b;
    typeof(*angle) t1;
    pos[0] += vel[0];
    pos[1] += vel[1];
    vel[1]++;
    {
        s32 c = ++(*((s16 *)(m + 0xA4E)));
        if (!((c & 0xFFFF) % 16)) {
            if (vel[0] > 0)
                vel[0]--;
            else if (vel[0] < 0)
                vel[0]++;
        }
    }
    if (pos[0] < 8) {
        pos[0] = 8;
        vel[0] = Halve(-vel[0]);
        RandomSpin_0816E6F0(spin);
    } else if (pos[0] > 0xE8) {
        pos[0] = 0xE8;
        vel[0] = Halve(-vel[0]);
        RandomSpin_0816E6F0(spin);
    }
    stopped = 0;
    if (pos[1] > 0x77) {
        s32 v;
        s32 zero;
        pos[1] = 0x78;
        v = Halve(-vel[1]);
        zero = 0;
        vel[1] = v;
        if (v > (-2)) {
            vel[0] = zero;
            vel[1] = zero;
            *spin = zero;
            if (((u16)((*angle) - 0x40)) < 0x80)
                *angle = 0x80;
            else
                *angle = zero;
            stopped = 1;
        } else {
            RandomSpin_0816E6F0(spin);
        }
    }
    if (stopped) {
        *((u32 *)(m + 0x2AA8)) &= ~2;
        {
            u16 t = pos[0] - 8;
            *((u16 *)(m + 0x2AC0)) = t;
        }
        {
            u16 t = pos[1] - 8;
            *((u16 *)(m + 0x2AC2)) = t;
        }
        {
            t1 = *angle;
            if ((*((u16 *)(m + 0x2AD6))) == 0)
                return 0;
            *((u16 *)(m + 0x2AD6)) = t1;
        }
        Anim_LoadKeyframe(m + 0x2AA0, m + 0xA4, 0xCE, 1);
        return 1;
    }
    *angle = (((*angle) + 0x100) + (*spin)) & 0xFF;
    a = Shift12(-(gUnk_086149C4[((*angle) + 0x40) & 0xFF] * 8));
    b = Shift12(-(gUnk_086149C4[*((u8 *)angle)] * 8));
    *((u32 *)(m + 0x2AA8)) |= 2;
    {
        u16 t = (pos[0] + b) + a;
        *((u16 *)(m + 0x2AC0)) = t;
    }
    {
        u16 t = (pos[1] + a) - b;
        *((u16 *)(m + 0x2AC2)) = t;
    }
    {
        u16 t = *angle;
        *((u16 *)(m + 0x2AD6)) = t;
    }
    return -1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0816E6F0);
#endif
