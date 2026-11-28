#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "player/player.h"
#include "scene/motion/scene_facing.h"
#include "scene/collision/contact_position.h"
#include "scene/collision/packed_contact_vector.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
static inline void sum(struct ContactPosition *d, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
// confidence=low: fires a player-homing projectile at a facing-dependent offset.
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u16, u8, u32);
void Projectile_SetHomingParameters(void *, s32, void *, s32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetScriptProcedure(void *, u32, u16);
void Projectile_SetCollisionParameters(void *, u32, u32, u32, u32, u32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, struct ContactPosition *);
void Ratatosk_UpdateHomingShot(struct Ratatosk *p, s32 tick)
{
    u8 params[0x58];
    struct PackedContactVector offset;
    struct ContactPosition spawn;
    struct Ratatosk *p2 = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 1, 1, 4);
    }
    if (tick == 28) {
        s32 direction = 0;
        switch (p->facing) {
            case 0:
                break;
            case 1:
                direction = 2;
                break;
            case 2:
                direction = 6;
                break;
        }
        Projectile_InitParameters(params);
        Projectile_SetLaunchParameters(params, 0, 180, (direction + 5) << 5, 24);
        Projectile_SetHomingParameters(params, 180, &gPlayers[0]->position, 4);
        Projectile_SetSpriteParameters(params, 0x8e5a, 0, 499);
        Projectile_SetScriptProcedure(params, 0x4363, 3);
        Projectile_SetCollisionParameters(params, 0, 0x80000, p2->wideAttack[0], p2->wideAttack[1],
                                          p2->wideAttack[2]);
        Projectile_SetBehaviorMode(params, 0);
        p2->gate++;
        switch (p->facing) {
            case 0:
                offset.x = 96;
                offset.y = 0;
                offset.z = -184;
                break;
            case 1:
                offset.x = 112;
                offset.y = 0;
                offset.z = -28;
                break;
            case 2:
                offset.x = -28;
                offset.y = 0;
                offset.z = 112;
                break;
            case 3:
                offset.x = -184;
                offset.y = 0;
                offset.z = 96;
                break;
        }
        sum(&spawn, (struct ContactPosition *)&offset, &p2->anchor);
        spawn.y += 150;
        Projectile_SpawnAtPosition(params, &spawn);
        Sound_PlaySong(0x56a);
    }
    if (tick > 30)
        p->actionFinished = 1;
    Callback_InvokeActor(p, p->actionTryUpdate);
}
