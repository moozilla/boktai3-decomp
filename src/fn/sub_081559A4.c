// confidence=low: inferred from player cue phases and animation consumers.
#include "player_cue.h"
// confidence=low: timed held-action phases, animation and completion script.
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_SetActionOrigin(void *);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void ConvergingEffects_EmitNearParticle(void *, void *);
void Hud_ClearTimer(void);
static inline u32 held(struct Player *p, u32 mask)
{
    return p->statusFlags & mask;
}
void Player_UpdateTimedChargeAction(struct Player *actor)
{
    struct Player *p = actor;
    Player_AddControlFlags(p, 8);
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            if ((p->inputKeys[1]) & 1)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1: {
            u16 *frame = &p->tetherFrame;
            if (!(*frame)) {
                p->facing = 4;
                p->heading = 1;
                p->side = 0;
                Player_SetActionOrigin(p);
                (*frame)++;
            }
            p->resetValue6FC = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1))
                break;
            Sound_PlaySong(203);
            if (!((p->inputKeys[0]) & 1))
                goto release;
            if (held(p, 16)) {
                Sound_PlaySong(216);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                ActorSequence_Begin(p, 7, 2);
                break;
            }
            p->chargeCounter = 0;
            goto pause;
        }

        case 2: {
            u32 flags = 160;
            u16 *count;
            p->statusFlags |= flags;
            count = &p->chargeCounter;
            if ((*count) <= 4) {
                if (((++(*count)) == 5) && (!(p->actionMode)))
                    Sound_PlaySong(296);
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->resetValue6FC = 30;
            ConvergingEffects_EmitNearParticle(p, &p->actionOrigin);
            if (!((p->inputKeys[0]) & 1)) {
                Sound_StopSong(216);
                p->chargeSoundActive = 0;
                goto release;
            }
            if (!held(p, 16)) {
                Sound_StopSong(216);
                p->chargeSoundActive = 0;
            pause:
                ActorSequence_Begin(p, 7, 3);

                goto end;
            }
            {
                s32 elapsed = gGameState->unk876;
                elapsed += PlayerCue_Timers(p)->elapsed;
                PlayerCue_Timers(p)->elapsed = elapsed;
                if ((u16)elapsed >= (PlayerCue_Timers(p)->required)) {
                    u32 *scriptPtr;
                    u32 script;
                    Hud_ClearTimer();
                    ActorSequence_Begin(p, 7, 5);
                    scriptPtr = &p->cueParameter;
                    script = *scriptPtr;
                    if (script) {
                        s32 value;
                        struct ScriptUtilityArguments call;
                        call.count = 1;
                        call.values = &value;
                        do {
                            value = 1;
                        } while (0);
                        *scriptPtr = 0;
                        Script_Call(script, &call);
                    }
                }
                break;
            }
        }

        case 3: {
            u32 flags = 128;
            p->statusFlags |= flags;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if (!((p->inputKeys[0]) & 1)) {
            release:
                ActorSequence_Begin(p, 7, 4);

                break;
            }
            if (held(p, 16)) {
                Sound_PlaySong(216);
                p->chargeSoundActive = 1;
                p->chargeCounter = 0;
                ActorSequence_Begin(p, 7, 2);
            }
            break;
        }
        case 4:
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 7, 0);
            }
            break;

        case 5:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->resetValue6FC = 30;
            ConvergingEffects_EmitNearParticle(p, &p->actionOrigin);
            break;
    }

end:
    if ((p->tetherPhase) != 5) {
        if (!(--(PlayerCue_Timers(p)->remaining))) {
            u32 *scriptPtr;
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(216);
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            scriptPtr = &p->cueParameter;
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
