// confidence=low: inferred from player cue phases and animation consumers.
#include "global.h"
// fix.py best (sweep after the inlined-insn fix)
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
void ConvergingEffects_EmitNearParticle(void *, void *);
void Hud_ClearTimer(void);
void Script_Call(u32, void *);
extern u8 *gUnk_02000710;
struct Call {
    u32 count : 16;
    u32 pad : 16;
    s32 *values;
};
struct ChargePosition {
    s16 x, y, z, pad;
};
struct TimedChargePlayer {
    u8 unk0[0x20];
    u32 flags;
    u8 unk24[0x324];
    const u16 *inputKeys;
    u8 unk34C[0x56];
    u8 heading;
    u8 side;
    u8 facing;
    u8 unk3A5[0x73];
    u8 actionMode;
    u8 unk419[0x3F];
    u8 phase;
    u8 unk459[0x1];
    u16 frame;
    u8 unk45C[0x9C];
    u16 count;
    u8 unk4FA[0x3];
    u8 soundActive;
    u8 unk4FE[0x2A];
    u16 timeLeft;
    u16 elapsed;
    u16 duration;
    u8 unk52E[0x76];
    u32 script;
    u8 unk5A8[0x154];
    u16 effectTimer;
    u8 unk6FE[0x382];
    struct ChargePosition origin;
};
static inline u32 held(struct TimedChargePlayer *p, u32 mask)
{
    return p->flags & mask;
}
void Player_UpdateTimedChargeAction(struct TimedChargePlayer *actor)
{
    struct TimedChargePlayer *p = actor;
    Player_AddControlFlags(p, 8);
    switch (p->phase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            if ((p->inputKeys[1]) & 1)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1: {
            u16 *frame = &p->frame;
            if (!(*frame)) {
                p->facing = 4;
                p->heading = 1;
                p->side = 0;
                Player_SetActionOrigin(p);
                (*frame)++;
            }
            p->effectTimer = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1))
                break;
            Sound_PlaySong(203);
            if (!((p->inputKeys[0]) & 1))
                goto release;
            if (held(p, 16)) {
                Sound_PlaySong(216);
                p->soundActive = 1;
                p->count = 0;
                ActorSequence_Begin(p, 7, 2);
                break;
            }
            p->count = 0;
            goto pause;
        }

        case 2: {
            u32 flags = 160;
            u16 *count;
            p->flags |= flags;
            count = &p->count;
            if ((*count) <= 4) {
                if (((++(*count)) == 5) && (!(p->actionMode)))
                    Sound_PlaySong(296);
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->effectTimer = 30;
            ConvergingEffects_EmitNearParticle(p, &p->origin);
            if (!((p->inputKeys[0]) & 1)) {
                Sound_StopSong(216);
                p->soundActive = 0;
                goto release;
            }
            if (!held(p, 16)) {
                Sound_StopSong(216);
                p->soundActive = 0;
            pause:
                ActorSequence_Begin(p, 7, 3);

                goto end;
            }
            {
                s32 elapsed = *((s16 *)(gUnk_02000710 + 0x876));
                elapsed += p->elapsed;
                p->elapsed = elapsed;
                if ((u16)elapsed >= (p->duration)) {
                    u32 *scriptPtr;
                    u32 script;
                    Hud_ClearTimer();
                    ActorSequence_Begin(p, 7, 5);
                    scriptPtr = &p->script;
                    script = *scriptPtr;
                    if (script) {
                        s32 value;
                        struct Call call;
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
            p->flags |= flags;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->effectTimer = 30;
            if (!((p->inputKeys[0]) & 1)) {
            release:
                ActorSequence_Begin(p, 7, 4);

                break;
            }
            if (held(p, 16)) {
                Sound_PlaySong(216);
                p->soundActive = 1;
                p->count = 0;
                ActorSequence_Begin(p, 7, 2);
            }
            break;
        }
        case 4:
            p->effectTimer = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 7, 0);
            }
            break;

        case 5:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 64, 1);
            p->effectTimer = 30;
            ConvergingEffects_EmitNearParticle(p, &p->origin);
            break;
    }

end:
    if ((p->phase) != 5) {
        if (!(--(p->timeLeft))) {
            u32 *scriptPtr;
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(216);
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            scriptPtr = &p->script;
            script = *scriptPtr;
            if (script) {
                s32 value;
                struct Call call;
                call.count = 1;
                call.values = &value;
                value = 0;
                *scriptPtr = 0;
                Script_Call(script, &call);
            }
        }
    }
}
