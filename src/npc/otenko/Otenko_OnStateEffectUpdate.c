#include "npc/otenko/otenko_actor.h"

#include "scene/collision/contact_position.h"
#include "engine/math/monster_particle_random.h"

extern const s16 gUnk_086149C4[];
void Actor_UpdateVisibilityAndShadow(u32, u8 *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void *SpriteEffectController_Create(const struct ContactPosition *, const struct ContactPosition *,
                                    s32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32,
                                    s32, s32, s32);
struct ParticleSampleOrigin {
    u32 value;
};
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline u32 IsEmissionFrame(u8 frame, u32 mask)
{
    if ((frame & mask) != 0)
        return 0;
    return frame < 128;
}
s32 Otenko_OnStateEffectUpdate(u32 context, struct OtenkoActor *actor)
{
    u32 work = context;
    struct OtenkoActor *npc = actor;
    struct OtenkoEffectState *fx = &npc->effect;
    if (fx->attached != 0) {
        struct ScenePoolVec32 *pos;
        struct ContactPosition *dst;
        s32 height;
        fx->projected = 1;
        pos = &npc->projection.position;
        dst = (struct ContactPosition *)npc->display.position;
        height = npc->projection.height;
        dst->x = (pos->x >> 12) - gSceneCamera->x;
        dst->y = ((pos->z + (pos->y + height)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
        {
            s16 *w = &npc->body.positionOffset.x;
            s16 height;
            *w = 4;
            w++;
            height = 0xFFE8;
            *w = height;
        }
        npc->body.position = *(const struct ContactPosition *)npc->display.position;
        npc->body.position.z = 0;
        {
            u32 m = 0x200;
            npc->displayFlags = m | npc->displayFlags;
        }
    }
    Actor_UpdateVisibilityAndShadow(work, (u8 *)npc);
    if (!fx->projected && npc->prevState <= 0x10) {
        u32 m = 0x402;
        npc->displayFlags = m | npc->displayFlags;
    }
    {
        u32 m = 4;
        if ((m & (npc->commandFlags)) != 0) {
            s32 t;
            if ((fx->paletteFrame) <= 0x3f) {
                u8 *g = gPaletteBankData;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = npc->workPalette;
                Palette_InterpolateBanks(d, s1, s2, fx->paletteFrame, 6);
            } else {
                u8 *g = gPaletteBankData;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = npc->workPalette;
                Palette_InterpolateBanks(d, s1, s2, 0x80 - (fx->paletteFrame), 6);
            }
            t = (((u16)fx->paletteFrame) + 1) & 0x7f;
            fx->paletteFrame = t;
            if (fx->attached != 0) {
                work = 3;
                if (!IsEmissionFrame(t, work))
                    return 0;
                {
                    u32 sheet = 0x1c1e, sequence = 0xd43f;
                    struct ContactPosition position;
                    struct ContactPosition velocityStorage;
                    struct ScenePoolVec32 *projected = &npc->projection.position;
                    struct ContactPosition *pos = &position;
                    s32 height = npc->projection.height;
                    pos->x = (projected->x >> 12) - gSceneCamera->x;
                    pos->y = ((projected->z + (projected->y + height)) >> 12) + 0x60;
                    pos->z = projected->z >> 12;
                    {
                        struct ParticleSampleOrigin origin, saved;
                        struct ContactPosition *velocityArgument;
                        const u16 *table;
                        struct ContactPosition *velocity;
                        s32 spread, magnitude, angle, speed, product, life, zero;
                        s32 horizontalScale, verticalScale;
                        origin.value =
                            (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
                        table = (const u16 *)RANDOM_TABLE_ADDR;
                        spread = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 63;
                        magnitude = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & work;
                        velocity = &velocityStorage;
                        angle = spread + 0x60;
                        speed = magnitude + 2;
                        horizontalScale = gUnk_086149C4[(spread + 0xa0) & 255];
                        product = horizontalScale * speed;
                        saved = origin;
                        velocityArgument = velocity;
                        velocity->x = Truncate(product);
                        verticalScale = gUnk_086149C4[angle];
                        velocity->y = Truncate(verticalScale * speed);
                        zero = 0;
                        velocity->z = 0;
                        life = (((s32)ParticleRandom_ReadRelativeSample(
                                     saved.value, (const u16 *)RANDOM_TABLE_ADDR) >>
                                 4) &
                                15) +
                               15;
                        SpriteEffectController_Create(&position, velocityArgument, sheet, sequence,
                                                      zero, 16, 17, 1, 2, 1, zero, life, zero, zero,
                                                      zero, zero, -4, -4);
                    }
                }
            }
        }
    }
    return 0;
}
