#include "global.h"
#include "player.h"
#include "engine.h"

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
u32 Player_ConsumeStateEntry(struct Player *);
void WirelessGame_NotifyPlayerDefeated(void);
void Player_AddControlFlags(struct Player *, u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u8, u32, u32, u32, u32, u32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateFacingSide(void *);
extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void RisingSprite_Spawn(struct SceneVec *);
void SparkParticle_Spawn(struct SceneVec *, s32, s32, s32, s32, s32, s32, s32);
void Camera_RequestShake(u32);
static inline s32 eighth(s32 v)
{
    if (v >= 0)
        return v >> 3;
    return -((-v) >> 3);
}
// confidence=low: follows a terrain target during recoil, then restores normal motion.
void Player_UpdateTerrainRecoil(struct Player *p)
{
    s32 x, z, angle, value;
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 64, 1);
        p->terrainRecoveryCounter = 0;
        if (p->contactActionTag) {
            Sound_PlaySong(p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    if (p->directionFrame) {
        if (p->terrainClampBlocked) {
            x = p->terrainTarget->x - p->position.x;
            z = p->terrainTarget->z - p->position.z;
            p->velocityX = eighth(x);
            p->velocityZ = eighth(z);
            if (p->unk461[0] <= 5) {
                if (!(p->unk461[0] & 1)) {
                    Sound_PlaySong(0x191);
                    RisingSprite_Spawn(&p->position);
                }
                p->unk461[0]++;
            }
        } else if (p->terrainRecoveryActive) {
            x = p->terrainTarget->x - p->position.x;
            z = p->terrainTarget->z - p->position.z;
            p->velocityX = eighth(x);
            p->velocityZ = eighth(z);
            if (p->unk461[0] <= 5) {
                if (!(p->unk461[0] & 1)) {
                    Sound_PlaySong(0x191);
                    SparkParticle_Spawn(&p->position, 1, 0, 0, 0, 12, 2, 2);
                }
                p->unk461[0]++;
            }
        } else if (p->actionActive489) {
            x = p->terrainTarget->x - p->position.x;
            z = p->terrainTarget->z - p->position.z;
            p->velocityX = eighth(x);
            p->velocityZ = eighth(z);
            p->unk461[0]++;
        } else {
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = -scale(value);
        }
        Camera_RequestShake(p->directionFrame);
        p->directionFrame = (p->directionFrame * 3) >> 2;
    } else {
        p->directionFrame = 0;
        p->terrainClampBlocked = 0;
        p->terrainRecoveryActive = 0;
        p->actionActive489 = 0;
        if (p->movementModifierKind == 3 && p->movementModifierActive) {
            p->unk461[0] = 0;
            ActorSequence_Begin(p, 0x16, 0);
        } else if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2A), 64, 1)) {
            Player_UpdateFacingSide(p);
            if (p->positionResetTimer && !(gEngineFlags & 0x1000)) {
                p->unk461[0] = 0;
                ActorSequence_Begin(p, 0x15, 0);
            } else {
                p->unk461[0] = 0;
                ActorSequence_Begin(p, 0, 0);
            }
        }
    }
}
