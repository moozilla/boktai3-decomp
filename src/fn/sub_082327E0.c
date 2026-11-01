#include "global.h"
#include "falling_objects.h"
#include "player_cue_sequence.h"
struct ColliderSpriteObject {
    u8 unknown0[2];
    u8 type, unknown3;
    u8 animationDone, remove;
    u8 unknown6[6];
    struct FallingVec position;
    u8 unknown14[0x28];
    struct FallingSprite sprite;
    struct FallingAnim anim;
    u8 unknown78[4];
    struct ColliderSpriteObject *next;
};
struct ColliderSpritePool {
    u8 task[0x1C];
    struct ColliderSpriteObject *head;
};
extern void *const gUnk_08E879CC[];
void ColliderSprite_Destroy(void *);
s32 Callback_InvokeActor(void *, void *);

s32 ColliderSpritePool_Update(struct ColliderSpritePool *pool)
{
    struct ColliderSpriteObject *p = pool->head, *next;
    for (; p; p = next) {
        next = p->next;
        if (p->remove) {
            ColliderSprite_Destroy(p);
            continue;
        }
        p->animationDone = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite,
                                                       (struct DisplaySequenceState *)&p->anim);
        p->sprite.position = p->position;
        Callback_InvokeActor(p, gUnk_08E879CC[p->type]);
    }
    return 0;
}
