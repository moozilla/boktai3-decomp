#include "global.h"
#include "script_utility.h"
#include "scene_vec.h"
// confidence=low: timed held-action phases, animation and completion script.
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_SetActionOrigin(void *);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void ConvergingEffects_EmitFarParticle(void *, void *);
void Hud_ClearTimer(void);
struct PlayerChargePulse {
    u8 unknown0[0x20];
    u32 statusFlags;
    u8 unknown24[0x324];
    const u16 *inputKeys;
    u8 unknown34C[0x56];
    u8 heading, side, facing;
    u8 unknown3A5[0x73];
    u8 actionMode;
    u8 unknown419[0x3F];
    u8 phase, unknown459;
    u16 timer;
    u8 unknown45C[0x9C];
    u16 chargeCounter;
    u8 unknown4FA[3];
    u8 chargeSoundActive;
    u8 unknown4FE[0x2A];
    u16 remainingFrames, elapsed, requiredFrames;
    u8 unknown52E[0x76];
    u32 completionScript;
    u8 unknown5A8[0x154];
    u16 effectDuration;
    u8 unknown6FE[0x382];
    struct SceneVec actionOrigin;
    u8 unknownA88[0x4E];
    u16 overlayResource, overlayIntensity;
};
static inline void AddStatusFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
static inline u32 TestStatusFlags(struct PlayerChargePulse *p, u32 mask)
{
    return p->statusFlags & mask;
}
// confidence=low: sibling of timed charge action with reversed hold gating and a pulsing overlay.
void PlayerCue_UpdateTimedChargePulse(struct PlayerChargePulse *p)
{
    Player_AddControlFlags(p, 8);
    switch (p->phase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            if (p->inputKeys[1] & 1)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1: {
            u16 *frame = &p->timer;
            if (!(*frame)) {
                p->facing = 4;
                p->heading = 1;
                p->side = 0;
                Player_SetActionOrigin(p);
                (*frame)++;
            }
            p->effectDuration = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1))
                break;
            Sound_PlaySong(0xCB);
            if (!(p->inputKeys[0] & 1))
                goto release;
            if (!TestStatusFlags(p, 16)) {
                Sound_PlaySong(0x2E4);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                ActorSequence_Begin(p, 7, 2);
                break;
            }
            p->chargeCounter = 0;
            goto pause;
        }

        case 2: {
            u16 *count;
            AddStatusFlags(&p->statusFlags, 0xA0);
            count = &p->chargeCounter;
            if ((*count) <= 4) {
                if ((++(*count)) == 5)
                    Sound_PlaySong(0x239);
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->effectDuration = 30;
            ConvergingEffects_EmitFarParticle(p, &p->actionOrigin);
            p->overlayResource = 0x16F;
            if ((p->timer) <= 31)
                p->overlayIntensity = p->timer;
            else
                p->overlayIntensity = 64 - (p->timer);
            if ((++(p->timer)) > 64)
                p->timer = 0;
            if (!(p->inputKeys[0] & 1)) {
                Sound_StopSong(0x239);
                p->chargeSoundActive = 0;
                goto release;
            }
            if (TestStatusFlags(p, 16)) {
                Sound_StopSong(0x239);
                p->chargeSoundActive = 0;
            pause:
                ActorSequence_Begin(p, 7, 3);

                goto end;
            }
            {
                if ((p->elapsed += 4) >= p->requiredFrames) {
                    u32 *scriptPtr;
                    u32 script;
                    Hud_ClearTimer();
                    p->phase = 5;
                    scriptPtr = &p->completionScript;
                    script = *scriptPtr;
                    if (script) {
                        s32 value;
                        struct ScriptUtilityArguments call;
                        call.count = 1;
                        call.values = &value;
                        value = 1;
                        *scriptPtr = 0;
                        Script_Call(script, &call);
                    }
                }
                break;
            }
        }

        case 3:
            AddStatusFlags(&p->statusFlags, 0x80);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->effectDuration = 30;
            if (!(p->inputKeys[0] & 1)) {
            release:
                ActorSequence_Begin(p, 7, 4);

                break;
            }
            if (!TestStatusFlags(p, 16)) {
                Sound_PlaySong(0x239);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                ActorSequence_Begin(p, 7, 2);
            }
            break;

        case 4:
            p->effectDuration = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 7, 0);
            }
            break;

        case 5:
            p->overlayResource = 0x16F;
            if ((p->timer) <= 31)
                p->overlayIntensity = p->timer;
            else
                p->overlayIntensity = 64 - (p->timer);
            if ((++(p->timer)) > 64)
                p->timer = 0;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->effectDuration = 30;
            ConvergingEffects_EmitFarParticle(p, &p->actionOrigin);
            break;
    }

end:
    if ((p->phase) != 5) {
        if (!(--(p->remainingFrames))) {
            u32 *scriptPtr;
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(0x239);
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            scriptPtr = &p->completionScript;
            script = *scriptPtr;
            if (script) {
                s32 value;
                struct ScriptUtilityArguments call;
                call.count = 1;
                call.values = &value;
                value = 0;
                *scriptPtr = 0;
                Script_Call(script, &call);
            }
        }
    }
}
