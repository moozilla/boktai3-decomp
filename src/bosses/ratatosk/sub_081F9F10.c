#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "player/player.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void Ratatosk_BeginDirectionalReaction(void);
void Ratatosk_UpdateDirectionalRecovery(void);
void Ratatosk_UpdateDefeatSequence(void);
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
                DamageNumberDisplay_Show(&p->hurtCollider, damage, 0, &p->body.position, &size, 1);
            }
            Ratatosk_StartAction(p, 1, 0, 2);
            Ratatosk_StartReaction(p, Ratatosk_BeginDirectionalReaction, 16);
            break;
        case 2:
            Ratatosk_StartAction(p, 1, 0, 3);
            Ratatosk_StartReaction(p, Ratatosk_UpdateDirectionalRecovery, 17);
            {
                u32 direction = body->direction;
                p->targetAngle = direction;
            }
            break;
        case 1: {
            p->targetAngle = p->player->unk2D;
            Ratatosk_SetFlags(p, 2);
            Ratatosk_StartAction(p, 1, 0, 1);
            Ratatosk_StartReaction(p, Ratatosk_UpdateDefeatSequence, 15);
        } break;
    }
    return 1;
}
