#include "global.h"

// confidence=low: five-phase action sequence for the Hresvelgr actor (state in +0x2F0); plays animations, emits burst particles and clears its per-frame halfword pair on exit.

// confidence=low: five-phase action sequence for the Hresvelgr actor (state in +0x2F0); plays animations, emits burst particles and clears its per-frame halfword triple on exit.

// Next value from the shared random table (table at 0x0203B400, index at 0x03005308).

// confidence=low: five-phase action sequence for the Hresvelgr actor (state in +0x2F0); plays animations, emits burst particles and clears its per-frame halfword triple on exit.

// Next value from the shared random table (table at 0x0203B400, index at 0x03005308).

// confidence=low: five-phase action sequence for the Hresvelgr actor (state in +0x2F0); plays animations, emits burst particles and clears its per-frame halfword triple on exit.
struct Sub081A14FCState {
    u8 state;
    u8 pad1[3];
    u32 phase;
};
struct Sub081A14FCSelf {
    u8 pad0[0x54];
    s16 x54;
    s16 x56;
    s16 x58;
    u8 pad5A[2];
    u32 w5c;
    u32 w60;
    u8 pad64[0x9c - 0x64];
    u32 flags9c;
    u8 pad_a0[3];
    u8 ua3;
    u8 pad_a4[0xaf - 0xa4];
    u8 uaf;
    u8 pad_b0;
    u8 ub1;
    u8 ub2;
    u8 pad_b3[0x2F0 - 0xb3];
    struct Sub081A14FCState st;
    u8 pad2F8[0x31C - 0x2F8];
    u16 h31c;
    u16 h31e;
    u16 h320;
    u16 h322;
    u8 pad32A[0x39C - 0x32A];
    u16 v39c[3];
    u8 pad3A2[0x3A4 - 0x3A2];
    u16 v3a4[3];
};
void ActionState_Reset(void *);
void ActionState_AdvanceState(void *);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, u32, u32, u32);
void SceneActors_RunSecondEventScript(void *, void *);
void Anim_SetFrameDurationScale(void *, u16);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void GameState_SetEventFlag(u32);
void AnimatedParticles_EmitBurst(void *, u32, u32, u32, u32, u32, u32, u32);
void MotionEntry_CreateWithDelta(void *, void *, u32);
s32 sub_08249420(u32, u32);
s32 sub_08249498(u32, u32);
s16 ScalarApproach_StepQuarter(s32, s32);
s32 TerrainModifiers_FindCellOverride(s32, s32);
void Sound_PlaySong(u32);
extern u8 *gUnk_030052F4;
extern s32 gUnk_030052F8;
extern s32 gUnk_030052FC;

// Next value from the shared random table (table at 0x0203B400, index at 0x03005308).
static inline u16 sub_081A14FC_Next(void)
{
    u32 *idx = (u32 *)0x03005308;
    *idx = (*idx + 1) & 0x3FF;
    return ((u16 *)RANDOM_TABLE_ADDR)[*idx];
}

#ifdef NONMATCHING
// fuzzy: 28.4%
// scope shape 589 (differences once low registers are set aside)
void sub_081A14FC(struct Sub081A14FCSelf *s, u32 frame)
{
    struct Sub081A14FCState *st;
    s32 result;
    s32 sp2c;

    st = &s->st;
    if (s->ub1) {
        s->ub1 = 0;
        ActionState_Reset(st);
    }

    switch (st->state) {
        case 0:
            if (st->phase == 0) {
                Hresvelgr_SetAnimation(s, (u8 *)s + 0x194, (u8 *)s + 0x174, 1, 0, 0xF);
                st->phase++;
                Sound_PlaySong(0x4CF);
                break;
            }
            if (s->ua3 > 8) {
                if (Player_IsSlotInActionTwelveOrThirtyEight(0) == 0) {
                    SceneActors_RunFirstEventScript(s, (u8 *)s + 0x1B4);
                    Hresvelgr_SetAnimation(s, (u8 *)s + 0x194, (u8 *)s + 0x174, 1, 0, 0x14);
                    Sound_PlaySong(0x3E2);
                    ActionState_AdvanceState(st);
                }
            }
            break;

        case 1:
            result = sub_08249420(st->phase, 0x1E);
            if (result == 8) {
                ActionState_AdvanceState(st);
                Anim_SetFrameDurationScale((u8 *)s + 0x194, 0x40);
                break;
            }
            if (sub_08249498(st->phase, 0x1E) != 0) {
                s->w5c = *(u32 *)&s->v3a4[0];
                s->w60 = *(u32 *)&s->v3a4[2];
                if (s->v3a4[1] > 0)
                    s->v3a4[1]--;
            } else {
                s32 dx, dz;
                s16 nx = ScalarApproach_StepQuarter(s->x54, 0xB54);
                s16 nz = ScalarApproach_StepQuarter(s->x58, 0xB54);
                s16 ny = ScalarApproach_StepQuarter(s->x56, 0xB54);
                s->v39c[0] = nx;
                s->v39c[1] = result;
                s->v39c[2] = nz;
                dx = (s->v39c[0] - s->x54) / 30;
                s->v3a4[1] = (u16)(0x3C / (st->phase + 1));
                dz = (s->v39c[2] - s->x58) / 30;
                s->v3a4[0] = dx;
                s->v3a4[2] = dz;
                Anim_SetFrameDurationScale((u8 *)s + 0x194, (u16)(((st->phase + 1) * 3 << 3) + 8));
                if (st->phase == 7)
                    Anim_SetKeyframes((u8 *)s + 0x194, (u8 *)s + 0x174, 0x14, 3, result);
            }
            if (sub_08249498(st->phase, (st->phase + 1) << 2) == 0) {
                s16 vec[3];
                s16 vec2[3];
                u16 a = sub_081A14FC_Next();
                u16 b = sub_081A14FC_Next();
                vec[0] = (a & 31) - 16;
                vec[1] = s->v3a4[1];
                vec[2] = (b & 31) - 16;
                vec2[0] = vec[0] + s->x54;
                vec2[1] = vec[1] + s->x56;
                vec2[2] = vec[2] + s->x58;
                MotionEntry_CreateWithDelta(vec2, vec, 0x40);
            }
            if (s->ub2)
                Sound_PlaySong(0x3D9);
            st->phase++;
            break;

        case 2:
            if (st->phase == 0) {
                SceneActors_StartUnmirroredAnimation(s, (u8 *)s + 0x194, (u8 *)s + 0x174, 1, 0,
                                                     0x15);
                st->phase++;
                Sound_PlaySong(0x3DD);
            }
            {
                s32 cx = (s8)(s->x54 >> 8);
                s32 cy = (s8)(s->x58 >> 8);
                s32 cell = 0;
                s32 override;
                u8 *p;
                u8 v;
                if (cx >= 0 && cy >= 0 && cx < gUnk_030052F8 && cy < gUnk_030052FC)
                    cell = ((u16 *)(gUnk_030052F4 + 0x24))[cy] + cx;
                override = TerrainModifiers_FindCellOverride(cell, 1);
                if (override != 0)
                    p = (u8 *) override + 4;
                else
                    p = *(u8 **)gUnk_030052F4 + 4 + cell * 4 + 0xC;
                v = *p;
                sp2c = (v & 0xF) << 8;
                if ((v >> 4) == 1)
                    sp2c -= *(u8 *)&s->x58;
                else if ((v >> 4) == 2)
                    sp2c -= *(u8 *)&s->x54;
            }
            if (s->x56 < sp2c) {
                s32 k;
                Sound_PlaySong(0x3DF);
                for (k = 1; k >= 0; k--) {
                    s16 burst[3];
                    u16 r0 = sub_081A14FC_Next();
                    u16 r1 = sub_081A14FC_Next();
                    burst[0] = (r0 >> 3) - 512 + s->x54;
                    burst[1] = (r1 >> 3) + s->x56;
                    burst[2] = (r0 >> 3) + s->x58;
                    AnimatedParticles_EmitBurst(burst, 7, 0, 0x40, 4, 4, 4, 4);
                }
                if (st->phase == 1)
                    SceneActors_StartUnmirroredAnimation(s, (u8 *)s + 0x194, (u8 *)s + 0x174, 2, 0,
                                                         0x16);
                s->x56 = (s16)sp2c;
                if (st->phase <= 3) {
                    st->phase++;
                    s->v3a4[1] = sub_08249420(0x20, st->phase);
                } else {
                    s->x56 = (s16)sp2c;
                    ActionState_AdvanceState(st);
                    goto finish;
                }
            }
            s->w5c = *(u32 *)&s->v3a4[0];
            s->w60 = *(u32 *)&s->v3a4[2];
            s->v3a4[1]--;
            if ((frame & 7) == 0 && st->phase == 1)
                AnimatedParticles_EmitBurst(&s->x54, 2, 0, 0x40, 8, 8, 4, 4);
            break;

        case 3:
            st->phase++;
            if (st->phase > 8) {
                ActionState_AdvanceState(st);
                GameState_SetEventFlag(0);
                GameState_SetEventFlag(3);
                GameState_SetEventFlag(0x4E);
                SceneActors_RunSecondEventScript(s, (u8 *)s + 0x1B4);
            }
            break;

        case 4:
            s->uaf = 1;
            break;
    }

finish:
    s->h31c = 0;
    s->h31e = 0;
    s->h320 = 0;
    s->h322 = 0;
    s->flags9c |= 0xE;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A14FC);
#endif
