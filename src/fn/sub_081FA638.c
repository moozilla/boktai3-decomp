#include "global.h"
struct RatatoskPosition {
    s16 x, y, z, pad;
};
struct RatatoskSprite {
    u8 pad0[12];
    struct RatatoskPosition position;
    u8 pad14[0x34];
};
struct Ratatosk {
    u8 pad0[0x48];
    struct RatatoskSprite sprite;
    s32 timer;
    u8 pad94[0x16];
    u8 flagAA;
    u8 padAB[0x1d];
    s32 targetDistanceSquared;
    u8 targetAngle;
    u8 padCD[0x43];
    struct RatatoskPosition *target;
    u8 pad114[0x218];
    s32 animationDelta;
    u8 pad330[0x1c];
    u8 animation[0x20];
    struct RatatoskPosition colliderPosition;
    u8 pad374[0xadc];
    s16 scale;
};
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
void Ratatosk_OscillateScale(struct Ratatosk *);
s32 SceneActors_UpdateAnimation(void *, void *, s32);
void sub_08225B74(void *);
void ActorProximity_UpdateSquaredDistances(struct Ratatosk *);
void Ratatosk_UpdatePointDistancesToPlayer(struct Ratatosk *);
void SceneDistance_UpdateLinkedDistances(struct Ratatosk *);
void SceneDistance_UpdateLinkedTargetDistances(struct Ratatosk *);
void sub_081F6B90(struct Ratatosk *);
void sub_081FA418(struct Ratatosk *);
void Ratatosk_UpdateAnimatedPosition(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    struct RatatoskSprite *sprite = &p->sprite;
    p->flagAA = 0;
    sub_081F6B90(p);
    sub_08225B74(sprite);
    if (Mod(p->timer, 30) == 0) {
        ActorProximity_UpdateSquaredDistances(p);
        SceneDistance_UpdateLinkedDistances(p);
    }
    if (Mod(p->timer, 30) == 15) {
        Ratatosk_UpdatePointDistancesToPlayer(p);
        SceneDistance_UpdateLinkedTargetDistances(p);
    }
    sub_081FA418(actor);
    SceneActors_UpdateAnimation(actor, actor->animation, (s32)&actor->animationDelta);
    Ratatosk_OscillateScale(actor);
    actor->colliderPosition.x = sprite->position.x;
    actor->colliderPosition.y = sprite->position.y + (actor->scale >> 4);
    actor->colliderPosition.z = sprite->position.z;
    if (actor->target) {
        s32 dx = actor->target->x - sprite->position.x;
        s32 dz = actor->target->z - sprite->position.z;
        actor->targetAngle = Math_Atan2(dx, dz);
        actor->targetDistanceSquared = dx * dx + dz * dz;
    }
}
