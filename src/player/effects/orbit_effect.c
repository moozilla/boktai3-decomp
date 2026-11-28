#include "player/effects/orbit_effect.h"
#include "engine/word_flags.h"
#include "engine/math/monster_particle_random.h"

void PlayerBurst_SetStateCallback(struct PlayerOrbitEffect *effect,
                                  void (*callback)(struct PlayerOrbitEffect *))
{
    effect->updateState = callback;
    effect->phaseFrame = 0;
}

extern const s16 gUnk_086149C4[];

static inline s32 TruncateOrbitOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

void PlayerOrbitEffect_UpdateParticle(struct PlayerOrbitEffect *effect,
                                      struct PlayerOrbitParticle *particle)
{
    particle->motion.angle += particle->motion.angularSpeed;
    particle->position.x = effect->owner->position.x +
                           TruncateOrbitOffset(particle->motion.radius *
                                               gUnk_086149C4[(particle->motion.angle + 64) & 0xFF]);
    particle->position.z =
        effect->owner->position.z +
        TruncateOrbitOffset(particle->motion.radius * gUnk_086149C4[particle->motion.angle]);
    if (effect->particleFrame >= particle->motion.expandDelay) {
        if ((effect->particleFrame & 7) == 0 && particle->motion.angularSpeed > 2)
            particle->motion.angularSpeed--;
        particle->motion.radius += 6;
        particle->position.y += particle->motion.riseSpeed;
    } else
        particle->motion.radius++;
    if (particle->motion.animationPhase == 0)
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 4);
    else if (particle->motion.animationPhase == 4)
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 5);
    particle->motion.animationPhase++;
    if (particle->motion.animationPhase > 7)
        particle->motion.animationPhase = 0;
}

void PlayerBurst_UpdateParticle(struct PlayerOrbitEffect *effect,
                                struct PlayerOrbitParticle *particle)
{
    particle->position.x += particle->motion.velocityX;
    particle->position.z += particle->motion.velocityZ;
    particle->motion.animationPhase++;
    if (particle->motion.animationPhase == 5)
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 5);
}

void PlayerOrbitEffect_SpawnParticle(struct PlayerOrbitEffect *effect,
                                     struct ContactPosition *position, s32 angle)
{
    s32 slotIndex;
    struct PlayerOrbitParticle *particle;
    for (slotIndex = 0; slotIndex < 16; slotIndex++) {
        if (!effect->particles[slotIndex].motion.active)
            break;
    }
    if (slotIndex < 16) {
        {
            const u16 *randomTable;
            u32 index;
            particle = &effect->particles[slotIndex];
            particle->flags &= ~1;
            particle->position = *position;
            randomTable = gRandomTable;
            index = (gRandomIndex + 1) & 0x3FF;
            {
                u32 random = ParticleRandom_ReadCursor(randomTable, &index);
                s32 x = (u16)particle->position.x;
                x -= 63;
                random &= 0x7F;
                particle->position.x = x + random;
            }
            index = (index + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = ParticleRandom_ReadCursor(randomTable, &index);
                s32 z = (u16)particle->position.z;
                z -= 63;
                random &= 0x7F;
                particle->position.z = z + random;
            }
        }
        particle->position.y += 128;
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 4);
        particle->motion.velocityX = TruncateOrbitOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        particle->motion.velocityZ = TruncateOrbitOffset(6 * gUnk_086149C4[angle & 255]);
        particle->motion.animationPhase = 0;
        particle->motion.state = 1;
        {
            u32 index = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = *(gRandomTable + index);
                particle->motion.lifetime = (random & 7) + 8;
            }
        }
        particle->motion.active = 1;
    }
}

void PlayerOrbitEffect_UpdateParticles(struct PlayerOrbitEffect *effect)
{
    s32 index;
    struct PlayerOrbitParticle *particle = effect->particles;
    struct PlayerOrbitParticleMotion *motion = &particle->motion;
    index = 0;
    do {
        if (motion->active) {
            if ((--motion->lifetime) == 0) {
                effect->particles[index].flags |= 1;
                motion->active = 0;
            } else if (motion->state == 0)
                PlayerOrbitEffect_UpdateParticle(effect, particle);
            else
                PlayerBurst_UpdateParticle(effect, particle);
        }
        particle++;
        motion = (struct PlayerOrbitParticleMotion *)((u8 *)motion + sizeof(*particle));
        index++;
    } while (index < 16);
    effect->particleFrame++;
}

void PlayerOrbitEffect_UpdateIntro(struct PlayerOrbitEffect *effect)
{
    struct Player *player;
    if (effect->phaseFrame == 0) {
        WordFlags_Set(4, &gEnginePauseFlags);
        Sound_PlaySong(0x302);
        Sound_PlaySong(0x1FA);
    }
    player = gPlayers[0];
    player->animation.flags |= 1;
    effect->drawable.position.y =
        (effect->drawable.position.y * 15 + player->position.y + 150) >> 4;
    Anim_Step(&effect->drawable, &effect->animationData);
    effect->phaseFrame++;
    if (effect->phaseFrame > 59) {
        effect->drawable.attribute1A = 1;
        PlayerBurst_SetStateCallback(effect, PlayerOrbitEffect_UpdateStatTransfer);
    }
}

// confidence=low: orbit animation drains current stat A to a target during the player transition.

void PlayerOrbitEffect_UpdateStatTransfer(struct PlayerOrbitEffect *effect)
{
    if (!effect->phaseFrame) {
        Sound_PlaySong(0x303);
        Sound_PlaySong(0x1FB);
    }
    gPlayers[0]->animation.flags |= 1;
    ActorShadow_Disable(gPlayers[0]->shadow);
    if (effect->phaseFrame <= 7) {
        if (effect->phaseFrame == 0) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE5, 1, 0);
        }
        effect->drawable.position.x = effect->owner->position.x - (effect->phaseFrame << 7);
        effect->drawable.position.z = effect->owner->position.z;
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x80);
    } else if (effect->phaseFrame <= 23) {
        if (effect->phaseFrame == 8) {
            effect->drawable.flags &= ~4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE5, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = effect->phaseFrame << 5;
            offset -= 768;
            effect->drawable.position.x = origin->x + offset;
        }
        effect->drawable.position.z = gCamera.position.z - ((effect->phaseFrame - 16) << 7);
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x48);
    } else if (effect->phaseFrame <= 39) {
        if (effect->phaseFrame == 24) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE6, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = effect->phaseFrame << 5;
            offset -= 1280;
            effect->drawable.position.x = origin->x - offset;
        }
        effect->drawable.position.z = gCamera.position.z + ((effect->phaseFrame - 32) << 7);
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0xC8);
    } else if (effect->phaseFrame <= 55) {
        if (effect->phaseFrame == 40) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE5, 1, 1);
        }
        effect->drawable.position.x = gCamera.position.x - ((effect->phaseFrame - 48) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = effect->phaseFrame << 5;
            offset -= 1280;
            effect->drawable.position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 9);
    } else if (effect->phaseFrame <= 71) {
        if (effect->phaseFrame == 56) {
            effect->drawable.flags &= ~4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE6, 1, 1);
        }
        effect->drawable.position.x = gCamera.position.x + ((effect->phaseFrame - 64) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = effect->phaseFrame << 5;
            offset -= 2304;
            effect->drawable.position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x89);
    } else {
        if (!(effect->phaseFrame & 3))
            PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0xC0);
        if (effect->phaseFrame == 72) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE6, 1, 1);
        }
        if (effect->phaseFrame == 96) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(&effect->drawable, &effect->animationData, 0xE8, 1, 0);
        }
        effect->drawable.position.x = effect->owner->position.x;
        effect->drawable.position.z = effect->owner->position.z - ((104 - effect->phaseFrame) << 5);
        if (effect->drawable.position.y > gPlayers[0]->position.y)
            effect->drawable.position.y -= 10;
        else
            effect->drawable.position.y = gPlayers[0]->position.y;
    }
    Anim_Step(&effect->drawable, &effect->animationData);
    if (effect->owner->currentStatA > effect->targetStatA) {
        s32 next = effect->owner->currentStatA - effect->recoveryStep;
        if (next < effect->targetStatA)
            effect->owner->currentStatA = effect->targetStatA;
        else
            effect->owner->currentStatA = next;
    }
    ++effect->phaseFrame;
    if (effect->phaseFrame > 104) {
        effect->drawable.attribute1A = 2;
        effect->owner->currentStatA = effect->targetStatA;
        PlayerBurst_SetStateCallback(effect, PlayerTransition_StepReveal);
    }
}
