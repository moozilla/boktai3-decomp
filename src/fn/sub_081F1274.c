#include "scene_departure_display.h"
#include "background_controller.h"
#include "palette_set.h"
#include "monster_particle_random.h"

typedef u16 Slot7PaletteBank[16];
extern Slot7PaletteBank gUnk_03005040[4];
void Collider_SetFixedPositionWithYOffset(void *, void *, s32);
void SpatialIndex_InsertEntry(void *);
void Anim_SetDurationScale(void *, u32);
void ScreenLayers_SelectRandomVariant(void *);
void ActorDisplay_RefreshScreenLayer(void *);
void SpriteParticle_InitParams(void *);
void SpriteParticle_SetMotionParams(void *, u32, u32, u32, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, s32);
void SpriteParticle_SetAnimationTrack(void *, u32, u32);
void SpriteParticle_SetPlanarVelocity(void *, s32, s32);
s32 SpriteParticle_Emit(void *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

#define RESTORE_BACKGROUND_PALETTES(actor, i, writeRow)                                            \
    do {                                                                                           \
        Slot7PaletteBank *readRow;                                                                 \
        u16 *bg = gBgPaletteBuffer;                                                                \
        u32 bank = 4;                                                                              \
        writeRow = (Slot7PaletteBank *)(bg + bank * 16);                                           \
        readRow = (actor)->savedPalettes;                                                          \
        for (i = 0; i < 4; i++)                                                                    \
            CpuSet(readRow + i, writeRow + i, 16);                                                 \
    } while (0)

#define UPDATE_FLASH_PALETTE(actor, writeRow)                                                      \
    {                                                                                              \
        s32 i;                                                                                     \
        for (i = 0; i < 4; i++)                                                                    \
            CpuSet((actor)->flashPalette, (u8 *)gBgPaletteBuffer + (i + 4) * 32, 16);              \
        (actor)->foregroundGraphics.paletteData = (u8 *)(actor)->flashPalette;                     \
        (actor)->accentGraphics.paletteData = (u8 *)(actor)->flashPalette;                         \
        (actor)->slowdownTimer--;                                                                  \
        if ((actor)->slowdownTimer == 0) {                                                         \
            RESTORE_BACKGROUND_PALETTES(actor, i, writeRow);                                       \
            AnimData_SelectPaletteBank(&(actor)->foregroundGraphics, (actor)->foregroundBank);     \
            AnimData_SelectPaletteBank(&(actor)->accentGraphics, (actor)->accentBank);             \
        }                                                                                          \
        if ((actor)->slowdownTimer & 1)                                                            \
            (actor)->shakeStrength = (actor)->slowdownTimer >> 2;                                  \
        else                                                                                       \
            (actor)->shakeStrength = -((actor)->slowdownTimer >> 2);                               \
    }

#define SET_RANDOM_EFFECT_TRACK(actor, params, paletteBank)                                        \
    {                                                                                              \
        if ((s32)(ParticleRandom_ReadSample(gRandomTable) & 255) > 128) {                          \
            SpriteParticle_SetSpriteResource(params, (actor)->effectResource, 0, (paletteBank));   \
            SpriteParticle_SetAnimationTrack(params, (actor)->effectGraphics, 5);                  \
        } else {                                                                                   \
            SpriteParticle_SetSpriteResource(params, (actor)->effectResource, 0, (paletteBank));   \
            SpriteParticle_SetAnimationTrack(params, (actor)->effectGraphics, 6);                  \
        }                                                                                          \
    }

static inline u32 ReadDamageSoundSample(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    offset = gRandomIndex * sizeof(u16);
    return *(const u16 *)(RANDOM_TABLE_ADDR + offset);
}

static inline u8 IsSceneHidden(void)
{
    if (gParticipantActors[0]) {
        u32 flags = gParticipantActors[0]->flags;
        u32 mask = 1;
        if (flags & mask)
            return 1;
    }
    return 0;
}

void SingletonSlot7_UpdatePresentation(struct DepartureActor *actor)
{
    struct DepartureActor *base = actor;
    s32 dx, dz;
    u8 *mode;
    Slot7PaletteBank *writeRow;
    u32 hidden;
    s32 i;
    if (actor->invincibilityTimer) {
        u32 mask = 4;
        actor->actorFlags = mask | actor->actorFlags;
        actor->invincibilityTimer--;
        if (actor->invincibilityTimer == 0) {
            u32 mask = ~4;
            actor->actorFlags = mask & actor->actorFlags;
        }
    }
    if (gSceneCamera) {
        Collider_SetFixedPositionWithYOffset(&actor->primaryFlags, &actor->position, 0);
        actor->hit = 0;
        {
            u32 *flags = &actor->secondaryFlags;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        Collider_SetFixedPositionWithYOffset(&actor->secondaryFlags, &actor->position, 0);
        SpatialIndex_InsertEntry(&actor->secondaryFlags);
    }
    hidden = IsSceneHidden();
    mode = &base->motionMode;
    if (hidden == 1 && *mode == 0)
        actor->primaryFlags |= 1;
    else {
        u32 *flags = &actor->primaryFlags;
        u32 mask = ~1;
        *flags = mask & *flags;
    }
    if (*mode) {
        if (actor->slowdownTimer > 0) {
            UPDATE_FLASH_PALETTE(actor, writeRow);
        } else {
            actor->shakeStrength = 0;
            {
                s32 *count = &actor->fadeFrame;
                u32 shift = actor->fadeShift;
                s32 limit = 1 << shift;
                if (*count < limit) {
                    s32 i;
                    (*count)++;
                    {
                        u32 fromBank = actor->foregroundFrom;
                        u8 *from = gPaletteBankData + fromBank * 32;
                        u8 *to = gPaletteBankData + actor->foregroundTo * 32;
                        u16 *dest = actor->foregroundPalette;
                        Palette_InterpolateBanks(dest, from, to, actor->fadeFrame,
                                                 actor->fadeShift);
                    }
                    actor->foregroundGraphics.paletteData = (u8 *)actor->foregroundPalette;
                    Anim_SetDurationScale(
                        &actor->foreground.sequence,
                        (u16)(Div(actor->fadeFrame * 192, 1 << actor->fadeShift) + 64));
                    {
                        u8 *from = gPaletteBankData + actor->accentFrom * 32;
                        u8 *to = gPaletteBankData + actor->accentTo * 32;
                        u16 *dest = actor->accentPalette;
                        Palette_InterpolateBanks(dest, from, to, actor->fadeFrame,
                                                 actor->fadeShift);
                    }
                    actor->accentGraphics.paletteData = (u8 *)actor->accentPalette;
                    for (i = 0; i <= 3; ++i)
                        Palette_InterpolateBanks(
                            gUnk_03005040[i], actor->paletteLinks.banks.from[i],
                            actor->paletteLinks.banks.to[i], actor->fadeFrame, actor->fadeShift);
                    if (actor->fadeFrame == (u32)(1 << actor->fadeShift)) {
                        Slot7PaletteBank *readRow;
                        writeRow = actor->savedPalettes;
                        readRow = gUnk_03005040;
                        for (i = 0; i < 4; i++) {
                            CpuSet(readRow, writeRow, 16);
                            ++writeRow;
                            ++readRow;
                        }
                    }
                    if (actor->fadeFrame == 120)
                        Sound_PlaySong(0x594);
                }
            }
        }
    } else if (actor->slowdownTimer > 0) {
        UPDATE_FLASH_PALETTE(actor, writeRow);
    }
    if (*mode) {
        ScreenLayers_SelectRandomVariant(base);
        ActorDisplay_RefreshScreenLayer(base);
        if (actor->burstTimer > 0) {
            u32 remainder = Mod(base->stateTimer, 6);
            if (!remainder) {
                u32 params[0x34 / sizeof(u32)];
                struct ContactPackedVector position;
                position.x = (ParticleRandom_ReadSample(gRandomTable) & 15) + actor->burstX - 8;
                position.y = (ParticleRandom_ReadSample(gRandomTable) & 7) + actor->burstY - 12;
                position.z = 100;
                dx = ((ParticleRandom_ReadSample(gRandomTable) & 15) - 8) << 10;
                dz = -(s32)((ParticleRandom_ReadSample(gRandomTable) & 7) << 10);
                SpriteParticle_InitParams(params);
                SpriteParticle_SetMotionParams(
                    params, 0, (ParticleRandom_ReadSample(gRandomTable) & 15) + 0x34, 0, remainder);
                if (actor->effectStyle == 0) {
                    SpriteParticle_SetSpriteResource(params, 0x4384, 0, 0x2A7);
                    SpriteParticle_SetAnimationTrack(params, 0xF62B, 0);
                } else if (actor->effectStyle == 1) {
                    SpriteParticle_SetSpriteResource(params, 0x61F9, 0, 0x2A6);
                    SpriteParticle_SetAnimationTrack(params, 0x31F4, 8);
                } else if (actor->effectStyle == 2) {
                    if ((s32)(ParticleRandom_ReadSample(gRandomTable) & 255) > 128) {
                        SpriteParticle_SetMotionParams(params, 0, 42, 0, remainder);
                        SET_RANDOM_EFFECT_TRACK(actor, params, 0x1F5);
                    } else {
                        SpriteParticle_SetSpriteResource(params, 0x61F9, 0, 0x1F5);
                        SpriteParticle_SetAnimationTrack(params, 0x31F4, 8);
                    }
                } else {
                    position.x = (ParticleRandom_ReadSample(gRandomTable) & 15) +
                                 actor->accent.sprite.position.x - 8;
                    position.y = (ParticleRandom_ReadSample(gRandomTable) & 7) +
                                 actor->accent.sprite.position.y;
                    position.z = 100;
                    SpriteParticle_SetMotionParams(params, 0, 42, 0, remainder);
                    SET_RANDOM_EFFECT_TRACK(actor, params, 2);
                }
                SpriteParticle_SetPlanarVelocity(params, dx, dz);
                SpriteParticle_Emit(params, &position);
            }
            actor->burstTimer--;
        }
    }
    if (actor->blinkActive != 0) {
        actor->blinkFrame++;
        Sprite_BlinkUntilExpired(&actor->accent, actor->blinkFrame, 45, 90);
    }
    if (actor->pendingDamage > 0 || actor->slowdownTimer == 0) {
        base->health -= actor->pendingDamage;
        if (actor->pendingDamage > 0) {
            struct ContactPackedVector offset;
            offset.x = 32;
            offset.y = 0;
            offset.z = 32;
            DamageNumberDisplay_Show(&actor->damageSource, actor->pendingDamage, 0,
                                     (const struct ContactPosition *)&actor->initialPosition,
                                     (const struct ContactPosition *)&offset, 1);
            if ((s32)(ReadDamageSoundSample() & 255) > 196)
                Sound_PlaySong(0x587);
            else
                Sound_PlaySong(0x13E);
        }
        if (base->health <= 0 && actor->phaseTimer2 == 0 && *mode == 1) {
            RESTORE_BACKGROUND_PALETTES(actor, i, writeRow);
            Script_Call(actor->setting, 0);
            actor->phaseTimer2 = 1;
        }
    }
    if (actor->damageTimer)
        actor->damageTimer--;
    if (*mode == 0 && actor->contactTimer) {
        actor->contactTimer--;
        if (actor->contactTimer == 0)
            actor->contactActive = 0;
    }
    actor->pendingDamage = 0;
    base->ticks++;
    base->motionRequest = 0;
    base->effectRequest = 0;
}
