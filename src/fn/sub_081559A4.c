// confidence=low: inferred from player cue phases and animation consumers.
#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 81.2%
// fix.py best (sweep after the inlined-insn fix)
// confidence=low: timed held-action phases, animation and completion script.
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
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
static inline u32 held(u8 *p, u32 mask)
{
    return W(0x20) & mask;
}
void Player_UpdateTimedChargeAction(u8 *actor)
{
    register u8 *p asm("r4") = actor;
    Player_AddControlFlags(p, 8);
    switch (*((u8 *)(p + 0x458))) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 0), 64, 1);
            if ((*((u16 *)((*((u32 *)(p + 0x348))) + 2))) & 1)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1: {
            u16 *frame = (u16 *)(p + 0x45A);
            if (!(*frame)) {
                *((u8 *)(p + 0x3A4)) = 4;
                *((u8 *)(p + 0x3A2)) = 1;
                *((u8 *)(p + 0x3A3)) = 0;
                Player_SetActionOrigin(p);
                (*frame)++;
            }
            *((u16 *)(p + 0x6FC)) = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 8), 64, 1))
                break;
            Sound_PlaySong(203);
            if (!((*((u16 *)(*((u32 *)(p + 0x348))))) & 1))
                goto release;
            if (held(p, 16)) {
                Sound_PlaySong(216);
                *((u8 *)(p + 0x4FD)) = 1;
                *((u16 *)(p + 0x4F8)) = 0;
                ActorSequence_Begin(p, 7, 2);
                break;
            }
            *((u16 *)(p + 0x4F8)) = 0;
            goto pause;
        }

        case 2: {
            u16 *count = (u16 *)(p + 0x4F8);
            *((u32 *)(p + 0x20)) |= 160;
            if ((*count) <= 4) {
                if (((++(*count)) == 5) && (!(*((u8 *)(p + 0x418)))))
                    Sound_PlaySong(296);
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 9), 64, 1);
            *((u16 *)(p + 0x6FC)) = 30;
            ConvergingEffects_EmitNearParticle(p, p + 0xA80);
            if (!((*((u16 *)(*((u32 *)(p + 0x348))))) & 1)) {
                Sound_StopSong(216);
                *((u8 *)(p + 0x4FD)) = 0;
                goto release;
            }
            if (!held(p, 16)) {
                Sound_StopSong(216);
                *((u8 *)(p + 0x4FD)) = 0;
            pause:
                ActorSequence_Begin(p, 7, 3);

                goto end;
            }
            {
                u32 elapsed = (u16)((*((s16 *)(gUnk_02000710 + 0x876))) + (*((u16 *)(p + 0x52A))));
                *((u16 *)(p + 0x52A)) = elapsed;
                if (elapsed >= (*((u16 *)(p + 0x52C)))) {
                    u32 script;
                    Hud_ClearTimer();
                    ActorSequence_Begin(p, 7, 5);
                    script = *((u32 *)(p + 0x5A4));
                    if (script) {
                        s32 value;
                        struct Call call;
                        call.count = 1;
                        call.values = &value;
                        do {
                            value = 1;
                        } while (0);
                        *((u32 *)(p + 0x5A4)) = 0;
                        Script_Call(script, &call);
                    }
                }
                break;
            }
        }

        case 3:
            *((u32 *)(p + 0x20)) |= 128;
            Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 10), 64, 1);
            *((u16 *)(p + 0x6FC)) = 30;
            if (!((*((u16 *)(*((u32 *)(p + 0x348))))) & 1)) {
            release:
                ActorSequence_Begin(p, 7, 4);

                break;
            }
            if (held(p, 16)) {
                Sound_PlaySong(216);
                *((u8 *)(p + 0x4FD)) = 1;
                *((u16 *)(p + 0x4F8)) = 0;
                ActorSequence_Begin(p, 7, 2);
            }
            break;

        case 4:
            *((u16 *)(p + 0x6FC)) = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 11), 64,
                                    1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 7, 0);
            }
            break;

        case 5:
            Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 9), 64, 1);
            *((u16 *)(p + 0x6FC)) = 30;
            ConvergingEffects_EmitNearParticle(p, p + 0xA80);
            break;
    }

end:
    if ((*((u8 *)(p + 0x458))) != 5) {
        if (!(--(*((u16 *)(p + 0x528))))) {
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(216);
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            script = *((u32 *)(p + 0x5A4));
            if (script) {
                s32 value;
                struct Call call;
                call.count = 1;
                call.values = &value;
                value = 0;
                *((u32 *)(p + 0x5A4)) = 0;
                Script_Call(script, &call);
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateTimedChargeAction);
#endif
