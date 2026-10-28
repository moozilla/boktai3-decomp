// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct TransitionSprite {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x20 - 0xC];
    u16 x, y;
    u8 unknown24[0x36 - 0x24];
    u16 angle;
};
struct MenuTransition {
    u8 unknown0[0xA4];
    u8 keyframes[0xA4E - 0xA4];
    u16 counter;
    u8 unknownA50[0x2AA0 - 0xA50];
    struct TransitionSprite sprite;
    u8 unknown2AD8[0x43EC - 0x2AD8];
    s16 position[2];
    u8 unknown43F0[4];
    s16 velocity[2];
    u8 unknown43F8[0xC];
    s16 spin;
    u16 angle;
};
#define TRANSITION_OFFSET(field) ((u32) & ((struct MenuTransition *)0)->field)

extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
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

inline static void RandomSpin_0816E6F0(s16 *spin)
{
    u16 *table = gUnk_0203B400;
    u32 *index = &gRandomIndex;
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
    s16 *pos = (s16 *)(m + TRANSITION_OFFSET(position));
    s16 *vel = (s16 *)(m + TRANSITION_OFFSET(velocity));
    u16 *angle = (u16 *)(m + TRANSITION_OFFSET(angle));
    s16 *spin = (s16 *)(m + TRANSITION_OFFSET(spin));
    s32 stopped;
    s32 a;
    s32 b;
    register u16 t1 asm("r0");
    pos[0] += vel[0];
    pos[1] += vel[1];
    vel[1]++;
    {
        register u32 counterMask asm("r2");
        register u32 phaseMask asm("r1");
        register u32 c asm("r0") = *((u16 *)(m + TRANSITION_OFFSET(counter))) + 1;
        *((u16 *)(m + TRANSITION_OFFSET(counter))) = c;
        counterMask = 0xFFFF;
        asm("" : "+r"(counterMask));
        phaseMask = counterMask;
        asm("" : "+r"(phaseMask));
        c &= phaseMask;
        c %= 16;
        if (!c) {
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
        *((u32 *)(m + TRANSITION_OFFSET(sprite.flags))) &= ~2;
        {
            u16 t = pos[0] - 8;
            *((u16 *)(m + TRANSITION_OFFSET(sprite.x))) = t;
        }
        {
            u16 t = pos[1] - 8;
            *((u16 *)(m + TRANSITION_OFFSET(sprite.y))) = t;
        }
        {
            t1 = *angle;
            *((u16 *)(m + TRANSITION_OFFSET(sprite.angle))) = t1;
            if (((u32)t1 << 16) == 0)
                return 0;
        }
        Anim_LoadKeyframe(m + TRANSITION_OFFSET(sprite), m + TRANSITION_OFFSET(keyframes), 0xCE, 1);
        return 1;
    }
    *angle = (((*angle) + 0x100) + (*spin)) & 0xFF;
    a = Shift12(-(gUnk_086149C4[((*angle) + 0x40) & 0xFF] * 8));
    b = Shift12(-(gUnk_086149C4[*((u8 *)angle)] * 8));
    *((u32 *)(m + TRANSITION_OFFSET(sprite.flags))) |= 2;
    {
        u16 t = (pos[0] + b) + a;
        *((u16 *)(m + TRANSITION_OFFSET(sprite.x))) = t;
    }
    {
        u16 t = (pos[1] + a) - b;
        *((u16 *)(m + TRANSITION_OFFSET(sprite.y))) = t;
    }
    {
        u16 t = *angle;
        *((u16 *)(m + TRANSITION_OFFSET(sprite.angle))) = t;
    }
    return -1;
}
