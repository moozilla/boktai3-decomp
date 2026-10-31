#include "player_cue.h"
#include "scene_vec.h"
// confidence=low: role inferred from callers and callback phases.
// confidence=low: approaches the scripted planar destination and returns to the standing update.
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Player_GetMovementRateScale(s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Sound_PlaySong(u32);

static inline s32 scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline s32 scale6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}
static inline u32 flags(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 slowed(struct Player *p)
{
    s32 active = p->active;
    s32 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
union PlayerCueReference {
    void *actor;
    struct Player *player;
};
static inline struct Player *PlayerCue_ResolveReference(void *actor)
{
    union PlayerCueReference reference;
    reference.actor = actor;
    return reference.player;
}
void Player_AdvanceScriptDestination(struct Player *p)
{
    s32 dx = (PlayerCue_Motion(p)->destination.x) - (p->position.x);
    s32 dz = (PlayerCue_Motion(p)->destination.z) - (p->position.z);
    s32 angle = Math_Atan2(dx, dz);
    s32 speed = 12;
    Player_GetMovementRateScale(12);
    if (p && slowed(p))
        speed = (speed * p->movementSpeedScale) >> 6;
    if (((dx * dx) + (dz * dz)) <= (speed * speed)) {
        p->position.x = PlayerCue_Motion(p)->destination.x;
        p->position.z = PlayerCue_Motion(p)->destination.z;
        if ((p->targetFacing) >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
        } else {
            s32 shifted = (((u16)angle) + 0x70) >> 5;
            u32 mask = 7;
            mask &= shifted;
            p->facing = mask;
        }
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    } else {
        p->velocityX = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
        p->velocityZ = scale12(gUnk_086149C4[angle & 255] * speed);
        if (p->velocityScaleAxis == 1) {
            if ((p->velocityZ) < 0)
                p->velocityZ = scale6((p->velocityZ) * 40);
            else
                p->velocityZ = scale6((p->velocityZ) * 52);
        } else if (p->velocityScaleAxis == 2) {
            if ((p->velocityX) < 0)
                p->velocityX = scale6((p->velocityX) * 40);
            else
                p->velocityX = scale6((p->velocityX) * 52);
        }
        p->facing = ((((angle + 16) & 255) >> 5) + 3) & 7;
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        if ((!((p->actionCueFlags) & 3)) && (!(p->actionCueCounter))) {
            if ((!((p && (p->active)) && (!(p->sceneMode)))) &&
                (!(p && ((u8)flags(&p->actionFlags, 4))))) {
                if (PlayerCue_ResolveReference(p) && slowed(p)) {
                    if (!(p->actionCueFlags))
                        Sound_PlaySong(0xC8);
                    else
                        Sound_PlaySong(0xC7);
                } else if ((p->tetherStatusFlags) || (!p->velocityScaleAxis)) {
                    if (!(p->actionCueFlags))
                        Sound_PlaySong(0xCA);
                    else
                        Sound_PlaySong(0xC9);
                } else if (!(p->actionCueFlags))
                    Sound_PlaySong(0x28E);
                else
                    Sound_PlaySong(0x28D);
            }
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 64, 1);
    }
}
