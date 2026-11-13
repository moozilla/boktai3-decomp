#include "converging_effects.h"

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

void SpriteDrawable_SelectTileFrame(struct ConvergingParticle *, void *, s32);

static inline s32 magnitude(s32 v)
{
    if (v < 0)
        return -v;
    return v;
}

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void ConvergingEffects_UpdateParticles(struct Player *player)
{
    s32 count, i;
    if (!player->convergingEffects.active)
        return;
    count = 0;
    for (i = 0; i < 4; i++) {
        struct ConvergingParticle *e = &player->convergingEffects.particles[i];
        if (e->active) {
            struct ConvergingParticle *draw = e;
            if (magnitude(draw->offsetX) <= 15 && magnitude(draw->offsetZ) <= 15) {
                draw->flags |= 1;
                e->active = 0;
            } else {
                s32 angle = Math_Atan2(e->offsetX, e->offsetZ);
                e->offsetX -= scale(e->speed * gUnk_086149C4[(angle + 64) & 255]);
                e->offsetZ -= scale(e->speed * gUnk_086149C4[angle & 255]);
                draw->position.x = player->actionOrigin.x + (u16)e->offsetX;
                draw->position.z = player->actionOrigin.z + (u16)e->offsetZ;
                if (++e->ticks == 4)
                    SpriteDrawable_SelectTileFrame(draw, player->convergingEffects.graphics,
                                                   e->kind + 1);
                count++;
            }
        }
    }
    if (!count)
        player->convergingEffects.active = 0;
}

static inline s32 ReadParticleSpeedSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
}

void ConvergingEffects_EmitNearParticle(struct Player *player, struct SceneVec *position)
{
    s32 step = gGameState->unk876;
    player->convergingEffects.ticks = step + 2 + player->convergingEffects.ticks;
    if (player->convergingEffects.ticks > 39) {
        struct ConvergingParticle *e =
            &player->convergingEffects.particles[player->convergingEffects.next];
        struct ConvergingParticle *draw = e;
        s32 angle;
        s32 radius;
        u16 radiusSample;
        draw->flags &= ~1;
        e->kind = 2;
        SpriteDrawable_SelectTileFrame(draw, player->convergingEffects.graphics, 2);
        angle = ParticleRandom_ReadDirectSample(gRandomTable) & 255;
        radiusSample = ParticleRandom_ReadDirectSample(gRandomTable);
        radius = radiusSample % 64 + 128;
        e->offsetX = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        e->offsetZ = scale(radius * gUnk_086149C4[angle & 255]);
        draw->position = *position;
        draw->position.x += e->offsetX;
        draw->position.z += e->offsetZ;
        e->speed = (ReadParticleSpeedSample() & 3) + 14;
        e->ticks = 0;
        e->active = 1;
        if (++player->convergingEffects.next > 3)
            player->convergingEffects.next = 0;
        player->convergingEffects.ticks -= 40;
        player->convergingEffects.active = 1;
    }
}

static inline s32 random_value(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)gRandomTable + (gRandomIndex << 1));
}

static inline s32 random_first(void)
{
    u8 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)(table + (gRandomIndex << 1));
}

void ConvergingEffects_EmitFarParticle(struct Player *player, struct SceneVec *position)
{
    if ((++player->convergingEffects.ticks & 3) == 0) {
        struct ConvergingParticle *e =
            &player->convergingEffects.particles[player->convergingEffects.next];
        struct ConvergingParticle *draw = e;
        s32 angle;
        s32 radius;
        draw->flags &= ~1;
        e->kind = 4;
        SpriteDrawable_SelectTileFrame(draw, player->convergingEffects.graphics, 4);
        angle = random_first() & 255;
        radius = (random_first() & 127) + 256;
        do {
            e->offsetX = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        } while (0);
        e->offsetZ = scale(radius * gUnk_086149C4[angle]);
        draw->position = *position;
        draw->position.x += (u16)e->offsetX;
        draw->position.z += (u16)e->offsetZ;
        e->speed = (random_value() & 7) + 28;
        e->ticks = 0;
        e->active = 1;
        player->convergingEffects.active = 1;
        if ((++player->convergingEffects.next) > 3) {
            player->convergingEffects.next = 0;
        }
    }
}

void ConvergingEffects_RemoveSprites(struct Player *player)
{
    struct ConvergingParticle *particle = player->convergingEffects.particles;
    s32 remaining;
    for (remaining = 3; remaining >= 0; remaining--) {
        Effect_Remove(particle);
        particle++;
    }
}

void ParticleSprite_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

void Effect_SetPalette(void *, u32);

void ConvergingEffects_InitSprites(struct Player *player)
{
    struct ConvergingEffectState *controller = &player->convergingEffects;
    s32 slot;
    controller->graphics = AnimData_FindEntry(0x1c1e);
    for (slot = 0; slot < 4; slot++) {
        struct ConvergingParticle *particle = &controller->particles[slot];
        ParticleSprite_Initialize(particle, controller->graphics, 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        particle->renderMode = 2;
    }
    controller->next = 0;
    controller->ticks = 0;
}
