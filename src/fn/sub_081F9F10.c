#include "global.h"
#include "ratatosk.h"
#include "player.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void Ratatosk_BeginDirectionalReaction(void);
void Ratatosk_UpdateDirectionalRecovery(void);
void Ratatosk_UpdateDefeatSequence(void);
static inline void setState(struct Ratatosk *p, u32 state, u32 next, u32 changed)
{
    p->actionMode = state;
    p->actionIndex = next;
    p->actionPhase = changed;
    p->actionTimer = 0;
    p->actionEnter = 1;
}
static inline void setCallback(struct Ratatosk *p, void (*callback)(void), u32 animation)
{
    p->actionState = animation;
    p->actionCallback = callback;
    p->callbackFrame = 0;
    p->actionChanged = 1;
    p->reactionPhase = 0;
    p->reactionTimer = 0;
}
s32 Ratatosk_ApplyDamageReaction(struct Ratatosk *p, s32 kind, s32 damage)
{
    struct SceneActorBody *body = &p->body;
    struct PackedVec size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show((u8 *)p + 0x188, damage, 0, (u8 *)p + 0x54, &size, 1);
            }
            setState(p, 1, 0, 2);
            setCallback(p, Ratatosk_BeginDirectionalReaction, 16);
            break;
        case 2:
            setState(p, 1, 0, 3);
            setCallback(p, Ratatosk_UpdateDirectionalRecovery, 17);
            {
                u32 direction = body->direction;
                p->targetAngle = direction;
            }
            break;
        case 1: {
            u32 direction = (u8)p->player->unk2D;
            u8 *dest = &p->targetAngle;
            u32 zero = 0;
            *dest = direction;
            {
                Ratatosk_SetFlags(p, 2);
            }
            setState(p, 1, zero, 1);
            setCallback(p, Ratatosk_UpdateDefeatSequence, 15);
        } break;
    }
    return 1;
}
