#include "global.h"
#include "ram.h"

struct ParticleVec {
    s16 x, y, z, pad;
};
struct ParticleEntry {
    struct ParticleVec position, velocity;
    u16 tag;
    u8 active;
    s8 index;
};
struct ParticleSprite {
    u32 flags;
    u8 pad4[11];
    u8 priority;
    u8 pad10[24];
};
struct ParticleManager {
    u8 task[24];
    void *resource;
    void *graphics;
    struct ParticleSprite sprites[32];
    u8 animations[32][16];
};

extern struct ParticleManager *gUnk_020001DC;
extern const u16 gUnk_086141BC[];
extern u8 *gGameState;

struct ParticleManager *EffectPool_CreateManager(void);
struct ParticleEntry *NativeParticle_AllocateEntry(struct ParticleManager *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);

static inline u16 random(u32 *seed)
{
    *seed = (*seed + 1) & 1023;
    return gUnk_086141BC[*seed];
}

static inline u8 night(void)
{
    u32 mask = 8;
    if (*(u32 *)(gGameState + 0x868) & mask)
        return 1;
    return 0;
}

s32 sub_08114E68(struct ParticleVec *position, s16 *range, s16 *spread, s32 count, u32 priority,
                 u32 animFlags, u32 spriteFlags, u32 tag)
{
    struct ParticleManager *m;
    struct ParticleSprite *sprite;
    s32 i;
    if (!position)
        return -1;
    m = gUnk_020001DC;
    if ((!m) && (!(m = EffectPool_CreateManager())))
        return -1;
    for (i = 0; i < count; i++) {
        struct ParticleEntry *e = NativeParticle_AllocateEntry(m);
        s8 sequence;
        u32 flag;
        if (!e)
            return -1;
        sprite = &m->sprites[e->index];
        {
            typeof(e->position) *q1 = &e->position;
            *q1 = *position;
            if (spread) {
                if (spread[0])
                    e->position.x +=
                        Mod(random(&gEffectRandomTableIndex), spread[0] * 2) - spread[0];
                if (spread[1])
                    e->position.y +=
                        Mod(random(&gEffectRandomTableIndex), spread[1] * 2) - spread[1];
            }
            if (!range) {
                e->velocity.x = 0;
                e->velocity.y = 0;
            } else {
                struct ParticleVec v;
                if (range[0] == 0)
                    v.x = 0;
                else
                    v.x = Mod(random(&gEffectRandomTableIndex), range[0]);
                if (range[1] == 0)
                    v.y = 0;
                else
                    v.y = Mod(random(&gEffectRandomTableIndex), range[1]);
                e->velocity = v;
            }
            ParticleSprite_Initialize(sprite, m->graphics, 0);
            Effect_SetOrigin(sprite, -8, -8);
            flag = night();
            sequence = 13;
            if (flag)
                sequence = 14;
            Anim_SetSequence(m->animations[e->index], m->resource, sequence, 0,
                             (u8)(Mod(random(&gEffectRandomTableIndex), 2) | animFlags));
            sprite->flags |= 0x11 | spriteFlags;
            sprite->priority = priority;
            e->active = 1;
            e->tag = tag;
        }
    }

    return 0;
}
