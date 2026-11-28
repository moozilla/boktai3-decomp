#include "player/player.h"
#include "scene/collision/contact_swarm.h"
#include "engine/math/scene_vec.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/vanargand/vanargand_projectiles.h"

void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32, u32);
void Camera_RequestShake(u32);
void RecoilPuff_Spawn(void *);
void Player_UpdateActionFacingSide(struct Player *);
extern const s16 gUnk_086149C4[];
struct PackedVec {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16, pad : 16;
};
// confidence=low: a landing cue that slides toward a terrain target, then returns to idle
static inline s32 Scale3(s32 v)
{
    if (v >= 0)
        return v >> 3;
    return -(-v >> 3);
}
void PlayerCue_UpdateTerrainSlide(struct Player *p)
{
    if (p->tetherMode != 0x1d) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag != 0) {
            Player_PlayLocalSong(p->slot, p->contactActionTag);
            p->contactActionTag = 0;
        }
        Player_SetTetherMode(p, 0x1d, 0);
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    if (p->directionFrame != 0) {
        if (p->terrainClampBlocked) {
            s32 dx = p->terrainTarget->x - p->position.x;
            s32 dz = p->terrainTarget->z - p->position.z;
            p->velocityX = Scale3(dx);
            p->velocityZ = Scale3(dz);
            if (p->tetherFrame <= 5) {
                if ((p->tetherFrame & 1) == 0) {
                    Player_PlayLocalSong(p->slot, 0x191);
                    RecoilPuff_Spawn(&p->position);
                }
                p->tetherFrame++;
            }
        } else {
            u32 ang = ((p->facing + 5) & 7) << 5;
            p->velocityX =
                -VanargandProjectile_Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * p->directionFrame);
            p->velocityZ = -VanargandProjectile_Scale(gUnk_086149C4[ang] * p->directionFrame);
        }
        Camera_RequestShake(p->directionFrame);
        p->directionFrame = (p->directionFrame * 3) >> 2;
    } else {
        p->directionFrame = 0;
        p->terrainClampBlocked = 0;
        p->contactSpecial = 0;
        if (p->movementModifierKind == 3 && p->movementModifierActive != 0) {
            Player_SetTetherMode(p, 0x16, 0);
        } else {
            Player_UpdateActionFacingSide(p);
            Player_SetTetherMode(p, 0, 0);
        }
    }
}
