#include "objects/moving_platforms/named_object.h"
#include "scene/collision/contact_actor.h"
void TrapSpellParticles_CreateDefaultBurst(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void SpriteParticle_ForwardCreate(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                  s32, s32, s32, s32, s32, s32);
// confidence=low: emits alternating particles at direction-dependent offsets while moving.
void MovingPlatform_EmitMovementTrail(struct NamedObject *object)
{
    struct ContactPosition offsets[2], position;
    s32 direction, width, height, depth;
    u32 kind;
    if (object->timer > 0) {
        --object->timer;
        return;
    }
    direction = object->body.direction >> 6;
    switch (direction) {
        case 0:
            offsets[0].x = -192;
            offsets[0].y = -192;
            offsets[0].z = 0;
            offsets[1].x = -192;
            offsets[1].y = -192;
            offsets[1].z = -128;
            depth = 62;
            width = 4;
            height = 32;
            break;
        case 1:
            offsets[0].x = -64;
            offsets[0].y = -192;
            offsets[0].z = -64;
            offsets[1].x = -220;
            offsets[1].y = -192;
            offsets[1].z = -64;
            depth = 64;
            width = 4;
            height = 32;
            break;
        case 2:
            offsets[0].x = -132;
            offsets[0].y = -192;
            offsets[0].z = -32;
            offsets[1].x = -132;
            offsets[1].y = -192;
            offsets[1].z = -192;
            depth = 62;
            width = 4;
            height = 24;
            break;
        case 3:
            offsets[0].x = -64;
            offsets[0].y = -192;
            offsets[0].z = -32;
            offsets[1].x = -220;
            offsets[1].y = -192;
            offsets[1].z = -32;
            depth = 64;
            width = 4;
            height = 32;
            break;
        default:
            return;
    }
    kind = object->kind;
    switch (kind) {
        case 0:
            if (object->trailPhase == 0) {
                position.x = object->body.position.x + offsets[0].x;
                position.y = object->body.position.y + offsets[0].y;
                position.z = object->body.position.z + offsets[0].z;
                TrapSpellParticles_CreateDefaultBurst(&position, 22, 8, width, 4, 24, direction,
                                                        height, depth, 3, 2, 512);
                object->trailPhase = 1;
            } else {
                position.x = object->body.position.x + offsets[1].x;
                position.y = object->body.position.y + offsets[1].y;
                position.z = object->body.position.z + offsets[1].z;
                TrapSpellParticles_CreateDefaultBurst(&position, 22, 8, width, 4, 24, direction,
                                                        height, depth, 3, 2, 512);
                object->trailPhase = 0;
            }
            break;
        case 3:
            if (object->trailPhase == 0) {
                position.x = object->body.position.x + offsets[0].x;
                position.y = object->body.position.y + offsets[0].y;
                position.z = object->body.position.z + offsets[0].z;
                SpriteParticle_ForwardCreate(&position, 22, 8, width, 4, 24, direction, height,
                                             depth, kind, 2, 512, 0, 0xF422, 0x121B, 0, 0, 0x128);
                object->trailPhase = 1;
            } else {
                position.x = object->body.position.x + offsets[1].x;
                position.y = object->body.position.y + offsets[1].y;
                position.z = object->body.position.z + offsets[1].z;
                SpriteParticle_ForwardCreate(&position, 22, 8, width, 4, 24, direction, height,
                                             depth, kind, 2, 512, 0, 0xF422, 0x121B, 0, 0, 0x128);
                object->trailPhase = 0;
            }
            break;
        default:
            return;
    }
    object->timer = 4;
}
