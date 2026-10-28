#include "global.h"
#include "render.h"
#include "player.h"
// confidence=low: orbit animation drains current stat A to a target during the player transition.
struct PlayerOrbitEffect {
    u8 unknown0[0x18];
    struct Player *owner;
    u8 animationData[0x20];
    u8 animation[0x1A];
    u8 animationState;
    u8 unknown57[5];
    struct SceneVec position;
    u8 unknown64[0x8E];
    u16 frame, recoveryStep, targetStatA;
};
void Sound_PlaySong(u32);
void ActorShadow_Disable(void *);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void PlayerOrbitEffect_SpawnParticle(void *, void *, s32);
s32 Anim_Step(void *, void *);
void PlayerTransition_StepReveal(void *);
void PlayerBurst_SetStateCallback(void *, void (*)(void *));
void PlayerOrbitEffect_UpdateStatTransfer(struct PlayerOrbitEffect *p)
{
    u16 *frame = &p->frame;
    void *animation;
    void *data;
    if (!*frame) {
        Sound_PlaySong(0x303);
        Sound_PlaySong(0x1FB);
    }
    gPlayers[0]->animation.flags |= 1;
    ActorShadow_Disable(gPlayers[0]->shadow);
    if (*frame <= 7) {
        animation = p->animation;
        data = p->animationData;
        if (*frame == 0) {
            *(u32 *)(p->animation + 8) |= 4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 0);
        }
        p->position.x = p->owner->position.x - (*frame << 7);
        p->position.z = p->owner->position.z;
        PlayerOrbitEffect_SpawnParticle(p, &p->position, 0x80);
    } else if (*frame <= 23) {
        animation = p->animation;
        data = p->animationData;
        if (*frame == 8) {
            *(u32 *)(p->animation + 8) &= ~4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 768;
            p->position.x = origin->x + offset;
        }
        p->position.z = gCamera.position.z - ((*frame - 16) << 7);
        PlayerOrbitEffect_SpawnParticle(p, &p->position, 0x48);
    } else if (*frame <= 39) {
        animation = p->animation;
        data = p->animationData;
        if (*frame == 24) {
            *(u32 *)(p->animation + 8) |= 4;
            Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
        }
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 1280;
            p->position.x = origin->x - offset;
        }
        p->position.z = gCamera.position.z + ((*frame - 32) << 7);
        PlayerOrbitEffect_SpawnParticle(p, &p->position, 0xC8);
    } else if (*frame <= 55) {
        animation = p->animation;
        data = p->animationData;
        if (*frame == 40) {
            *(u32 *)(p->animation + 8) |= 4;
            Anim_SetKeyframes(animation, data, 0xE5, 1, 1);
        }
        p->position.x = gCamera.position.x - ((*frame - 48) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 1280;
            p->position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(p, &p->position, 9);
    } else if (*frame <= 71) {
        animation = p->animation;
        data = p->animationData;
        if (*frame == 56) {
            *(u32 *)(p->animation + 8) &= ~4;
            Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
        }
        p->position.x = gCamera.position.x + ((*frame - 64) << 7);
        {
            struct CameraPosition *origin = &gCamera.position;
            s32 offset = *frame << 5;
            offset -= 2304;
            p->position.z = origin->z + offset;
        }
        PlayerOrbitEffect_SpawnParticle(p, &p->position, 0x89);
    } else {
        struct SceneVec *pos;
        u32 bits = *frame & 3;
        pos = &p->position;
        if (!bits)
            PlayerOrbitEffect_SpawnParticle(p, &p->position, 0xC0);
        {
            u32 current = *frame;
            animation = p->animation;
            data = p->animationData;
            if (current == 72) {
                *(u32 *)(p->animation + 8) |= 4;
                Anim_SetKeyframes(animation, data, 0xE6, 1, 1);
            }
        }
        if (*frame == 96) {
            *(u32 *)(p->animation + 8) |= 4;
            Anim_SetKeyframes(animation, data, 0xE8, 1, 0);
        }
        pos->x = p->owner->position.x;
        p->position.z = p->owner->position.z - ((104 - *frame) << 5);
        if (p->position.y > gPlayers[0]->position.y)
            p->position.y -= 10;
        else
            p->position.y = gPlayers[0]->position.y;
    }
    Anim_Step(animation, data);
    if (p->owner->currentStatA > p->targetStatA) {
        s32 next = p->owner->currentStatA - p->recoveryStep;
        if (next < p->targetStatA)
            p->owner->currentStatA = p->targetStatA;
        else
            p->owner->currentStatA = next;
    }
    ++p->frame;
    if (p->frame > 104) {
        p->animationState = 2;
        p->owner->currentStatA = p->targetStatA;
        PlayerBurst_SetStateCallback(p, PlayerTransition_StepReveal);
    }
}
