// MATCHING_CONCESSION: register-pinning
#include "contact_sequence_actor.h"
#define ContactSequenceActor_ApplyHealthContact sub_08045D54
#define ContactSequenceActor_SetState           sub_08045D30
void ContactSequenceActor_SetState(struct ContactSequenceActor *, u32);
void sub_0821FF8C(void *, void *);
void sub_08159860(void *, void *, u32);
void sub_080148CC(u32, u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_080472A4(void *, u32, s32, void *, const struct ContactPackedVector *, u32);
void ContactSequenceActor_ApplyHealthContact(struct ContactCollider *source,
                                             struct ContactCollider *other,
                                             struct ContactSequenceActor *actor)
{
    struct ContactPackedVector size;
    register struct ContactSequenceActor *saved asm("r8");
    struct ContactPosition *position;
    saved = actor;
    sub_0821FF8C(source, other);
    if (actor->cooldown == 0) {
        sub_08159860(source, other, actor->reactionValue);
        actor->cooldown = 10;
        actor->health -= other->cost;
        actor->flash = 8;
        if (actor->health <= 0) {
            actor->health = 0;
            ContactSequenceActor_SetState(actor, 3);
        }
        position = &saved->position;
        sub_080148CC(2, 14, position, 64, 256, 12, 8, 8, 8, 30, 30);
        size.x = 16;
        size.y = 0;
        size.z = 16;
        sub_080472A4(other, other->cost, 0, position, &size, 1);
        Sound_PlaySongChecked(0x3B9);
    }
}
