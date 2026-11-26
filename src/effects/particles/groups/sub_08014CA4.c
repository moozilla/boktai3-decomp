#include "effects/particles/fragment_group.h"
#include "engine/math/scene_vec_polar.h"
// confidence=low: allocates a fragment group with randomized direction and lifetime.
#define FragmentGroup_SpawnRadial sub_08014CA4
extern u16 gUnk_08604C42[], gUnk_08604C30[];
s32 Mod(s32, s32);
static inline s32 Random(const u16 *table, u32 *rng)
{
    u32 r = (*rng + 1) & 0x3ff;
    *rng = r;
    return table[r];
}
static inline s32 RandomByte(const u16 *table, u32 *rng)
{
    u32 r = (*rng + 1) & 0x3ff;
    *rng = r;
    return *(u8 *)&table[r];
}
s32 FragmentGroup_SpawnRadial(s32 count, s32 type, const struct ContactPosition *position,
                              s32 radius, s32 lifetime, s32 lifetimeRange, s32 speed,
                              s32 speedRange, s32 angle, s32 angleRange, s32 vertical,
                              s32 verticalRange)
{
    struct FragmentManager *m;
    struct FragmentGroup *g;
    struct FragmentGroup *base;
    u16 *speeds;
    u32 *rng;
    s32 i;
    struct SpriteRenderResource *animation;
    u8 *cursor;
    u8 *lifetimes;
    s32 j;
    struct SpriteRenderResource *animData;
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
    animData = &m->groups[0].animation;
    base = g;
    for (; j < 4; g++, speeds = (void *)(((u8 *)speeds) + sizeof(struct FragmentGroup)),
                       lifetimes += sizeof(struct FragmentGroup),
                       animData = (void *)((u8 *)animData + sizeof(struct FragmentGroup)), base++,
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
                AnimData_SelectPaletteBank(animation, g->animation.paletteKey + 1);
            {
                i = 0;
                if (i < count) {
                    const u16 *table;
                    struct SceneVec *velocity;
                    u16 *speedOut;
                    u8 *life;
                    table = (const u16 *)(0x0203B400 + (g->type >> 8));
                    cursor = (u8 *)base;
                    velocity = base->velocity;
                    speedOut = speeds;
                    life = lifetimes;
                    do {
                        struct DisplaySequenceSprite *p;
                        s32 direction;
                        u8 *out;
                        p = &base->sprites[i];
                        ((struct FragmentGroup *)((u32)base + (base->active >> 8)))
                            ->initialized[i] = 1;
                        if (!lifetimeRange) {
                            u8 value = lifetime;
                            out = life;
                            *out = value;
                        } else {
                            u32 *seed = rng;
                            u32 index;
                            s32 random, value;
                            index = (*seed + 1) & 1023;
                            *seed = index;
                            random = table[index];
                            value = lifetime + Mod(random, lifetimeRange);
                            out = life;
                            *out = value;
                        }
                        {
                            s32 spread = speedRange;
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
                            ((struct FragmentGroup *)cursor)->velocity[0].y = value;
                        }
                        p->flags &= ~1;
                        p->position = *position;
                        p->position.x += SceneVec_TruncateComponent(velocity->x * radius);
                        p->position.z += SceneVec_TruncateComponent(velocity->z * radius);
                        Sprite_Init(p, animation, 0x200);
                        p->frame = gUnk_08604C30[g->type];
                        if (RandomByte(table, rng) <= 128)
                            p->flags |= 4;
                        else
                            p->flags &= ~4;
                        ((struct FragmentGroup *)((u32)base + (base->active >> 8)))->enabled[i] = 1;
                        cursor += 8;
                        velocity++;
                        speedOut++;
                        life++;
                        i++;
                    } while (i < count);
                }
            }
            g->active = 1;
            break;
        }
    }

    return 0;
}
