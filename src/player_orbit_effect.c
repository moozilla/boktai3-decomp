#include "player_orbit_effect.h"

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
    s32 value, delta;
    s32 valueZ, deltaZ;
    particle->motion.angle += particle->motion.angularSpeed;
    value = particle->motion.radius * gUnk_086149C4[(particle->motion.angle + 64) & 0xFF];
    delta = TruncateOrbitOffset(value);
    particle->position.unsignedCoordinates.x = (u16)effect->owner->position.x + delta;
    valueZ = particle->motion.radius * gUnk_086149C4[particle->motion.angle];
    deltaZ = TruncateOrbitOffset(valueZ);
    particle->position.unsignedCoordinates.z = (u16)effect->owner->position.z + deltaZ;
    if (effect->particleFrame >= particle->motion.expandDelay) {
        if ((effect->particleFrame & 7) == 0 && particle->motion.angularSpeed > 2)
            particle->motion.angularSpeed--;
        particle->motion.radius += 6;
        particle->position.unsignedCoordinates.y += particle->motion.riseSpeed;
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
    {
        u16 x = particle->motion.velocityX;
        u16 y = particle->position.unsignedCoordinates.x;
        particle->position.unsignedCoordinates.x = x + y;
    }
    {
        u16 x = particle->motion.velocityZ;
        u16 y = particle->position.unsignedCoordinates.z;
        particle->position.unsignedCoordinates.z = x + y;
    }
    particle->motion.animationPhase++;
    if (particle->motion.animationPhase == 5)
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 5);
}

void PlayerOrbitEffect_SpawnParticle(struct PlayerOrbitEffect *effect, struct SceneVec *position,
                                     s32 angle)
{
    s32 slotIndex;
    struct PlayerOrbitParticle *particle;
    for (slotIndex = 0; slotIndex < 16; slotIndex++) {
        if (!effect->particles[slotIndex].motion.active) {
            break;
        }
    }
    if (slotIndex < 16) {
        {
            const u16 *randomTable;
            u32 *randomIndex;
            u32 range;
            u32 mask;
            u32 index;
            particle = &effect->particles[slotIndex];
            particle->flags &= ~1;
            particle->position.coordinates = *position;
            randomTable = gRandomTable;
            randomIndex = &gRandomIndex;
            index = *randomIndex;
            index++;
            mask = 0x3FF;
            index &= mask;
            {
                u32 random = randomTable[*(&index)];
                s32 x = (u16)particle->position.coordinates.x;
                x -= 63;
                range = 0x7F;
                random &= range;
                particle->position.coordinates.x = x + random;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            {
                u32 random = randomTable[index];
                s32 z = (u16)particle->position.coordinates.z;
                z -= 63;
                random &= range;
                particle->position.coordinates.z = z + random;
            }
        }
        particle->position.coordinates.y += 128;
        SpriteDrawable_SelectTileFrame(particle, effect->resource, 4);
        particle->motion.velocityX = TruncateOrbitOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        particle->motion.velocityZ = TruncateOrbitOffset(6 * gUnk_086149C4[angle & 255]);
        particle->motion.animationPhase = 0;
        particle->motion.state = 1;
        {
            u32 index = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = *(u16 *)((index << 1) + (u32)gRandomTable);
                u32 range = 7;
                particle->motion.lifetime = (random & range) + 8;
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
        u32 mask = 4;
        gEnginePauseFlags |= mask;
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
        effect->drawable.state = 1;
        PlayerBurst_SetStateCallback(effect, PlayerOrbitEffect_UpdateStatTransfer);
    }
}

// confidence=low: orbit animation drains current stat A to a target during the player transition.

void PlayerOrbitEffect_UpdateStatTransfer(struct PlayerOrbitEffect *effect)
{
    u16 *frame = &effect->phaseFrame;
    struct PlayerOrbitDrawable *animation;
    struct PlayerOrbitResource *data;
    if (!*frame) {
        Sound_PlaySong(0x303);
        Sound_PlaySong(0x1FB);
    }
    gPlayers[0]->animation.flags |= 1;
    ActorShadow_Disable(gPlayers[0]->shadow);
    if (*frame <= 7) {
        animation = &effect->drawable;
        data = &effect->animationData;
        if (*frame == 0) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 0);
        }
        effect->drawable.position.x = effect->owner->position.x - (*frame << 7);
        effect->drawable.position.z = effect->owner->position.z;
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x80);
    } else if (*frame <= 23) {
        animation = &effect->drawable;
        data = &effect->animationData;
        if (*frame == 8) {
            effect->drawable.flags &= ~4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 768;
            effect->drawable.position.x = origin->x + offset;
        }
        effect->drawable.position.z = gCamera.position.z - ((*frame - 16) << 7);
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x48);
    } else if (*frame <= 39) {
        animation = &effect->drawable;
        data = &effect->animationData;
        if (*frame == 24) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 1280;
            effect->drawable.position.x = origin->x - offset;
        }
        effect->drawable.position.z = gCamera.position.z + ((*frame - 32) << 7);
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0xC8);
    } else if (*frame <= 55) {
        animation = &effect->drawable;
        data = &effect->animationData;
        if (*frame == 40) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 1);
        }
        effect->drawable.position.x = gCamera.position.x - ((*frame - 48) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 1280;
            effect->drawable.position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 9);
    } else if (*frame <= 71) {
        animation = &effect->drawable;
        data = &effect->animationData;
        if (*frame == 56) {
            effect->drawable.flags &= ~4;
            Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
        }
        effect->drawable.position.x = gCamera.position.x + ((*frame - 64) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 2304;
            effect->drawable.position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0x89);
    } else {
        struct SceneVec *position;
        u32 bits = *frame & 3;
        position = &effect->drawable.position;
        if (!bits)
            PlayerOrbitEffect_SpawnParticle(effect, &effect->drawable.position, 0xC0);
        {
            u32 current = *frame;
            animation = &effect->drawable;
            data = &effect->animationData;
            if (current == 72) {
                effect->drawable.flags |= 4;
                Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
            }
        }
        if (*frame == 96) {
            effect->drawable.flags |= 4;
            Anim_SetKeyframes(animation, data, 0xE8, 1, 0);
        }
        position->x = effect->owner->position.x;
        effect->drawable.position.z = effect->owner->position.z - ((104 - *frame) << 5);
        if (effect->drawable.position.y > gPlayers[0]->position.y)
            effect->drawable.position.y -= 10;
        else
            effect->drawable.position.y = gPlayers[0]->position.y;
    }
    Anim_Step(animation, data);
    if (effect->owner->currentStatA > effect->targetStatA) {
        s32 next = effect->owner->currentStatA - effect->recoveryStep;
        if (next < effect->targetStatA)
            effect->owner->currentStatA = effect->targetStatA;
        else
            effect->owner->currentStatA = next;
    }
    ++effect->phaseFrame;
    if (effect->phaseFrame > 104) {
        effect->drawable.state = 2;
        effect->owner->currentStatA = effect->targetStatA;
        PlayerBurst_SetStateCallback(effect, PlayerTransition_StepReveal);
    }
}
