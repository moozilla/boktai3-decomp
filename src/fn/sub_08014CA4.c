// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "engine_globals.h"
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
extern u16 gUnk_08604C42[], gUnk_08604C30[];
extern s16 gUnk_086149C4[];
#define RANDOM_TABLE ((u16 *)0x0203B400)
void *FragmentManager_Create(void *, u32);
s32 AnimData_Load(void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Sprite_Init(struct Sprite *, void *, s32);
s32 Mod(s32, s32);
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
    register u16 *speeds asm("sl");
    register u32 *rng asm("sb");
    s32 i;
    u8 *animation;
    u8 *cursor;
    u8 *lifetimes;
    s32 j;
    u8 *animData;
    if ((!gFragmentManager) && (!FragmentManager_Create(0, 0)))
        return -1;
    m = gFragmentManager;
    if (count > 6)
        count = 6;
    j = 0;
    rng = &gRandomIndex;
    g = m->groups;
    speeds = m->groups[0].speed;
    lifetimes = m->groups[0].lifetime;
    animData = m->groups[0].animation;
    base = g;
    for (; j < 4; g++, speeds = (void *)(((u8 *)speeds) + sizeof(struct Group)),
                       lifetimes += sizeof(struct Group), animData += sizeof(struct Group), base++,
                       j++) {
        if (!g->active) {
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
                struct Vec *velocity;
                u16 *speedOut;
                register u8 *life asm("sl");
                cursor = (u8 *)base;
                velocity = base->velocity;
                speedOut = speeds;
                life = lifetimes;
                asm("" : "+r"(table), "+r"(velocity), "+r"(life));
                do {
                    struct Sprite *p;
                    s32 direction;
                    p = &base->sprites[i];
                    base->initialized[i] = 1;
                    if (!lifetimeRange) {
                        register u8 *out asm("r1");
                        register u8 value asm("r2") = lifetime;
                        asm("" : "+r"(value));
                        out = life;
                        *out = value;
                    } else {
                        register u32 *seed asm("r2") = rng;
                        u32 index;
                        s32 random, value;
                        index = (*seed + 1) & 1023;
                        *seed = index;
                        random = table[index];
                        value = lifetime + Mod(random, lifetimeRange);
                        *life = value;
                    }
                    {
                        register s32 spread asm("r2") = speedRange;
                        asm("" : "+r"(spread));
                        if (!spread) {
                            u32 value = (u16)speed;
                            *speedOut = value;
                        } else {
                            s32 random = Random(table, rng);
                            *speedOut = speed + Mod(random, speedRange);
                        }
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
                        s32 value = vertical + Mod(random, verticalRange);
                        ((struct Group *)cursor)->velocity[0].y = value;
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
                    i++;
                } while (i < count);
            }
            g->active = 1;
            break;
        }
    }

    return 0;
}
