#include "global.h"
// confidence=low: allocates a fragment group with randomized direction and lifetime.
#define FragmentGroup_SpawnRadial sub_08014CA4
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad4[12];
    u16 tile;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Group {
    u8 active, count, type, pad3;
    u8 animation[28];
    u8 initialized[6], lifetime[6], enabled[6];
    u16 speed[6];
    u8 pad3e[2];
    struct Vec velocity[6];
    struct Sprite sprites[6];
};
struct Manager {
    u8 pad[32];
    struct Group groups[4];
};
extern struct Manager *gUnk_02000064;
extern u32 gRandomIndex;
extern u16 gUnk_08604C42[], gUnk_08604C30[];
extern s16 gUnk_086149C4[];
#define RANDOM_TABLE ((u16 *)0x0203B400)
void *FragmentManager_Create(void *, u32);
s32 AnimData_Load(void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Sprite_Init(struct Sprite *, void *, s32);
s32 Mod(s32, s32);
#ifdef NONMATCHING
// fuzzy: 89.2%
// fix.py best (sweep after the inlined-insn fix)
// Animation-pointer retention and array-cursor allocation still differ.
static inline s32 Random(u16 *table, u32 *rng)
{
    u32 r = (*rng + 1) & 0x3ff;
    *rng = r;
    return table[r];
}
static inline s32 RandomByte(u16 *table, u32 *rng)
{
    u32 r = (*rng + 1) & 0x3ff;
    *rng = r;
    return *(u8 *)&table[r];
}
static inline s32 Scale(s32 a, s32 b)
{
    s32 v = a * b;
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
s32 FragmentGroup_SpawnRadial(s32 count, s32 type, struct Vec *position, s32 radius, s32 lifetime,
                              s32 lifetimeRange, s32 speed, s32 speedRange, s32 angle,
                              s32 angleRange, s32 vertical, s32 verticalRange)
{
    struct Manager *m;
    struct Group *g;
    register struct Group *base asm("r8");
    u8 *lifetimes;
    u16 *speeds;
    register u32 *rng asm("sb");
    s32 i;
    s32 j;
    u8 *animData;
    typeof(lifetimes) t1;
    typeof(rng) t2;
    if ((!gUnk_02000064) && (!FragmentManager_Create(0, 0)))
        return -1;
    m = gUnk_02000064;
    if (count > 6)
        count = 6;
    j = 0;
    rng = (t2 = &gRandomIndex);
    g = m->groups;
    speeds = (void *)(((u8 *)m) + 0x52);
    {
        register u16 *held asm("sl") = speeds;
        speeds = held;
    }
    lifetimes = (t1 = ((u8 *)m) + 0x46);
    animData = ((u8 *)m) + 0x24;
    base = g;
    for (; j < 4; g++, base++, lifetimes += sizeof(struct Group),
                  speeds = (void *)(((u8 *)speeds) + (sizeof(struct Group))),
                  animData += sizeof(struct Group), j++) {
        if (!g->active) {
            u8 *animation;
            s32 loaded;
            g->type = type;
            g->count = count;
            loaded = AnimData_Load(animData, gUnk_08604C42[g->type]);
            animation = animData;
            if (!loaded)
                return -1;
            if ((g->type == 4) || (g->type == 8))
                AnimData_SelectPaletteBank(animation, (*((u16 *)(g->animation + 6))) + 1);
            i = 0;
            if (i < count) {
                u16 *table = (u16 *)0x0203B400;
                u8 *cursor = (u8 *)base;
                struct Vec *velocity = base->velocity;
                u16 *speedOut = speeds;
                register u8 *life asm("sl") = t1;
                asm("" : "+r"(table), "+r"(velocity), "+r"(life));
                do {
                    struct Sprite *p;
                    s32 direction;
                    asm("" : "+r"(base));
                    p = &base->sprites[i];
                    base->initialized[i] = 1;
                    if (!lifetimeRange)
                        *life = lifetime;
                    else {
                        s32 random = Random(table, t2);
                        *life = lifetime + Mod(random, lifetimeRange);
                    }
                    if (!speedRange)
                        *speedOut = speed;
                    else {
                        s32 random = Random(table, rng);
                        *speedOut = speed + Mod(random, speedRange);
                    }
                    if (!angleRange)
                        direction = angle;
                    else {
                        s32 random = Random(table, rng);
                        direction = angle + Mod(random, angleRange);
                    }
                    velocity->x = gUnk_086149C4[(direction + 64) & 255];
                    velocity->z = gUnk_086149C4[direction & 255];
                    if (!verticalRange)
                        velocity->y = vertical;
                    else {
                        s32 random = Random(table, rng);
                        *((s16 *)(cursor + 0x42)) = vertical + Mod(random, verticalRange);
                    }
                    p->flags &= ~1;
                    p->position = *position;
                    p->position.x += Scale(velocity->x, radius);
                    p->position.z += Scale(velocity->z, radius);
                    Sprite_Init(p, animation, 0x200);
                    p->tile = gUnk_08604C30[g->type];
                    if (RandomByte(table, rng) <= 128)
                        p->flags |= 4;
                    else
                        p->flags &= ~4;
                    base->enabled[i] = 1;
                    cursor += 8;
                    velocity++;
                    speedOut++;
                    life++;
                    asm("" : "+r"(life));
                    i++;
                } while (i < count);
            }
            g->active = 1;
            break;
        }
    }

    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08014CA4);
#endif
