#include "scene/collision/contact_swarm.h"
#include "player/player.h"
#include "player/cues/player_cue.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/vanargand/vanargand_projectiles.h"
// confidence=low: provisional action interpretation from retained C and assembly.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void RecoilPuff_Spawn(void *);
void SparkParticle_Spawn(void *, s32, s32, s32, s32, s32, s32, s32);
void Camera_RequestShake(u32);
extern const s16 gUnk_086149C4[];
static inline void ApproachTerrainTarget(struct Player *p)
{
    s32 x = p->terrainTarget->x - p->position.x;
    s32 z = p->terrainTarget->z - p->position.z;
    if (x >= 0)
        p->velocityX = x >> 3;
    else
        p->velocityX = -(-x >> 3);
    if (z >= 0)
        p->velocityZ = z >> 3;
    else
        p->velocityZ = -(-z >> 3);
}
// confidence=low: moves toward a saved effect position or recoils while shake decays.
void Player_UpdateRecoilMotion(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Sound_PlaySong(p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if (p->directionFrame) {
        if (p->terrainClampBlocked) {
            ApproachTerrainTarget(p);
            if (p->tetherFrame <= 5) {
                if (!(p->tetherFrame & 1)) {
                    Sound_PlaySong(0x191);
                    RecoilPuff_Spawn(&p->position);
                }
                p->tetherFrame++;
            }
        } else if (p->terrainRecoveryActive) {
            ApproachTerrainTarget(p);
            if (p->tetherFrame <= 5) {
                if (!(p->tetherFrame & 1)) {
                    Sound_PlaySong(0x191);
                    SparkParticle_Spawn(&p->position, 1, 0, 0, 0, 12, 2, 2);
                }
                p->tetherFrame++;
            }
        } else if (p->actionActive489) {
            ApproachTerrainTarget(p);
            p->tetherFrame++;
        } else {
            s32 angle = ((p->facing + 5) & 7) << 5;
            p->velocityX =
                -VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * p->directionFrame);
            p->velocityZ = -VanargandProjectile_Scale(gUnk_086149C4[angle] * p->directionFrame);
        }
        Camera_RequestShake(p->directionFrame);
        p->directionFrame = p->directionFrame * 3 >> 2;
    } else {
        p->directionFrame = 0;
        p->terrainClampBlocked = 0;
        p->terrainRecoveryActive = 0;
        p->actionActive489 = 0;
        p->contactSpecial = 0;
        if (p->movementModifierKind == 3 && p->movementModifierActive)
            ActorSequence_Begin(p, 22, 0);
        else {
            Player_UpdateFacingSide(p);
            ActorSequence_Begin(p, 0, 0);
        }
    }
}
