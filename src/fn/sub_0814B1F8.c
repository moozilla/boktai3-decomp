#include "global.h"
#include "ram.h"

extern u8 *gUnk_02000538;
extern u32 gUnk_0200051C;
extern u32 gUnk_02000524;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];

#define PL_AT(T, off) (*(T *)((u8 *)p + (off)))
/* position of the current interaction target (x at +0, z at +2, y at +4) */
#define TGT_POS() ((s16 *)(*(u8 **)gUnk_02000538 + 0xC))

s32 Player_ConsumeStateEntry(void *);
void *ParticlePool_Create(void);
void *sub_0814B078(void *, s32, s32);
void Player_OnTargetMotionExit(void);
void Player_SetPendingCallback(void *, void *);
void WirelessGame_ResetCyclingItemLatch(void);
void Player_EmitQueuedEffects(void *);
void Player_NotifyActionCategories(void *);
void Player_AddControlFlags(void *, u32);
void Sound_PlaySong(u32);
void Player_SetActionPhase(void *, u32);
void Player_SetFacingFromAngle(void *, s32);
s32 Math_Atan2(s32, s32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_Step(void *, void *);
void ParticlePool_EmitTriple(void *, void *, s32, s32);
void SceneMovingParticle_Create(void *, void *, s32, s32);
void PlayerCue_InitAttackCollider(void *, void *, s32, s32);
s32 Div(s32, s32);
void TemporaryAnimationPool_CreateEntry(void *, s32, s32, s32, s32);
void Player_UpdateFacingSide(void *);
void ActorSequence_Begin(void *, s32, s32);

/* fixed-point multiply result, truncating toward zero */
static inline s32 Player_0814B1F8_Fix(s32 v)
{
    if (v < 0)
        return -((-v) >> 12);
    return v >> 12;
}

/* two-keyframe-set pick by facing side (0x3A2), then flip flag from 0x3A3 */
#define PLAYER_START_ANIM(A0, A1, N, F)                                                            \
    do {                                                                                           \
        if (PL_AT(u8, 0x3A2) == 0)                                                                 \
            Anim_SetKeyframes((u8 *)p + 0xEC, (u8 *)p + 0x6C, A0, N, F);                           \
        else                                                                                       \
            Anim_SetKeyframes((u8 *)p + 0xEC, (u8 *)p + 0x6C, A1, N, F);                           \
        if (PL_AT(u8, 0x3A3))                                                                      \
            PL_AT(u32, 0xEC + 8) |= 4;                                                             \
        else                                                                                       \
            PL_AT(u32, 0xEC + 8) &= ~4;                                                            \
    } while (0)

static inline s32 Player_0814B1F8_Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return gUnk_0203B400[gRandomIndex];
}

#ifdef NONMATCHING
// fuzzy: 38.7%
// scope shape 863 (differences once low registers are set aside)
// confidence=low: player "throw/bolt at target" state machine: 8-phase switch (approach, dash, strike, return)
void sub_0814B1F8(void *p)
{
    s32 dx, dy, ang, cnt, m, q;
    s16 vel[3];
    s16 vel2[3];
    s16 pos[4];
    s16 *tp;
    s32 r0, r1, r2;

    if (Player_ConsumeStateEntry(p)) {
        if (PL_AT(void *, 0xA88) == 0)
            PL_AT(void *, 0xA88) = ParticlePool_Create();
        PL_AT(u16, 0xA9E) = 0x32;
        gUnk_02000538 = sub_0814B078(p, 1000000, 0);
        gUnk_0200051C = 0;
        gUnk_02000524 = 0;
        PL_AT(u32, 0xA8C) = PL_AT(u32, 0x30);
        PL_AT(u32, 0xA90) = PL_AT(u32, 0x34);
        PL_AT(u32, 0x10C) = PL_AT(u32, 0xAC);
        PL_AT(u32, 0x110) = PL_AT(u32, 0xB0);
        PL_AT(u32, 0xEC + 8) &= ~2;
        Player_SetPendingCallback(p, Player_OnTargetMotionExit);
        if (gEngineFlags & 0x1000)
            WirelessGame_ResetCyclingItemLatch();
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }

    PL_AT(u16, 0x126) = PL_AT(u16, 0xC6);
    PL_AT(u32, 0x134) = PL_AT(u32, 0xD4);
    Player_AddControlFlags(p, 9);
    PL_AT(u32, 0x20) = (PL_AT(u32, 0x20) & ~0x4000) | 0x60000;

    switch (PL_AT(u8, 0x458)) {
        case 0:
            if (gUnk_02000538 == 0) {
                Sound_PlaySong(0x124);
                Player_SetActionPhase(p, 5);
                break;
            }
            if (PL_AT(u16, 0x45A) == 0) {
                tp = TGT_POS();
                dx = tp[0] - PL_AT(s16, 0x10C);
                tp = TGT_POS();
                dy = tp[2] - PL_AT(s16, 0x110);
                Player_SetFacingFromAngle(p, Math_Atan2(dx, dy));
                PLAYER_START_ANIM(0x1A1, 0x1A6, 1, 0);
            }
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            PL_AT(u16, 0x45A)++;
            if (PL_AT(u16, 0x45A) <= 10)
                break;
            goto phase1_exit;

        case 1:
            tp = TGT_POS();
            dx = tp[0] - PL_AT(s16, 0x10C);
            tp = TGT_POS();
            dy = tp[2] - PL_AT(s16, 0x110);
            ang = Math_Atan2(dx, dy);
            ParticlePool_EmitTriple(PL_AT(void *, 0xA88), (u8 *)p + 0x10C, (ang + 0x80) & 0xFF, 0);
            cnt = PL_AT(u16, 0x45A);
            if (cnt == 0) {
                Player_SetFacingFromAngle(p, ang);
                Sound_PlaySong(0x4F5);
                PLAYER_START_ANIM(0x1A2, 0x1A7, 1, 0);
            }
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            if (dx * dx + dy * dy < PL_AT(u16, 0xA9E) * PL_AT(u16, 0xA9E)) {
                PL_AT(u8, 0x3A4) = (((ang + 0x20) & 0xFF) >> 6) * 2 + 3 & 7;
                PL_AT(u16, 0x10E) = TGT_POS()[1];
                Player_SetFacingFromAngle(p, ang);
                Player_SetActionPhase(p, 2);
                r0 = Player_0814B1F8_Rand() & 0xFF;
                r1 = (Player_0814B1F8_Rand() & 0x1F) - 0x10;
                vel[0] = Player_0814B1F8_Fix(gUnk_086149C4[(r0 + 0x40) & 0xFF] * r1);
                vel[1] = 0;
                vel[2] = Player_0814B1F8_Fix(gUnk_086149C4[r0] * r1);
                vel[1] = (Player_0814B1F8_Rand() & 0x3F) * 2 - 0x30;
                {
                    s32 *src = (s32 *)TGT_POS();
                    s32 pp[2];
                    pp[0] = src[0];
                    pp[1] = src[1];
                    ((s16 *)pp)[1] += 0x1E;
                    SceneMovingParticle_Create(pp, vel, 0x3C, 1);
                }
            } else {
                PL_AT(u16, 0x10C) +=
                    Player_0814B1F8_Fix(gUnk_086149C4[(ang + 0x40) & 0xFF] * PL_AT(u16, 0xA9E));
                PL_AT(u16, 0x110) +=
                    Player_0814B1F8_Fix(gUnk_086149C4[ang & 0xFF] * PL_AT(u16, 0xA9E));
                PL_AT(u8, 0x3A4) = (((ang + 0x20) & 0xFF) >> 6) * 2 + 3 & 7;
                if (Div(PL_AT(u16, 0xA9C), 2) < PL_AT(u16, 0x45A)) {
                    PL_AT(u16, 0x10E) -= 0x50;
                    if (PL_AT(s16, 0x10E) < TGT_POS()[1])
                        PL_AT(u16, 0x10E) = TGT_POS()[1];
                } else {
                    PL_AT(u16, 0x10E) += 0x50;
                    if (PL_AT(s16, 0x10E) > TGT_POS()[1] + 1000)
                        PL_AT(u16, 0x10E) = TGT_POS()[1] + 1000;
                }
                q = ((ang + 0x60) & 0xFF) >> 6;
                if (q > 1) {
                    PL_AT(u8, 0x3A2) = 3 - q;
                    PL_AT(u8, 0x3A3) = 1;
                } else {
                    PL_AT(u8, 0x3A2) = q;
                    PL_AT(u8, 0x3A3) = 0;
                }
                PL_AT(u16, 0x45A)++;
                if (PL_AT(u16, 0x45A) > 0x59)
                    Player_SetActionPhase(p, 6);
            }
            if (*(u16 *)(PL_AT(u8 *, 0x348) + 2) & 2)
                gUnk_02000524 = 1;
            if (PL_AT(u8, 0x45F) == 1 && (*(u16 *)(PL_AT(u8 *, 0x348) + 2) & 1))
                gUnk_02000524 = 1;
            PlayerCue_InitAttackCollider(p, (u8 *)p + 0x10C, 1, 0x60);
            break;

        case 2:
            if (PL_AT(u16, 0x45A) == 0) {
                Sound_PlaySong(0x4FB);
                tp = TGT_POS();
                dx = tp[0] - PL_AT(s16, 0x10C);
                tp = TGT_POS();
                dy = tp[2] - PL_AT(s16, 0x110);
                if (dx * dx + dy * dy > 0x9C4) {
                    ang = Math_Atan2(dx, dy);
                    Player_SetFacingFromAngle(p, ang);
                    PL_AT(u16, 0x10C) =
                        TGT_POS()[0] +
                        Player_0814B1F8_Fix(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                    PL_AT(u16, 0x110) =
                        TGT_POS()[2] + Player_0814B1F8_Fix(gUnk_086149C4[ang & 0xFF] * 0x32);
                }
                gUnk_0200051C = 0;
            }
            if (PL_AT(u16, 0x45A) == 5)
                PLAYER_START_ANIM(0x1A3, 0x1A8, 2, 0);
            if ((PL_AT(u16, 0x45A) & 1) == 0) {
                r0 = Player_0814B1F8_Rand() & 0xFF;
                r1 = (Player_0814B1F8_Rand() & 0x1F) - 0x10;
                vel2[0] = Player_0814B1F8_Fix(gUnk_086149C4[(r0 + 0x40) & 0xFF] * r1);
                vel2[1] = 0;
                vel2[2] = Player_0814B1F8_Fix(gUnk_086149C4[r0] * r1);
                vel2[1] = (Player_0814B1F8_Rand() & 0x3F) * 2 - 0x30;
                {
                    s32 *src = (s32 *)TGT_POS();
                    s32 pp[2];
                    pp[0] = src[0];
                    pp[1] = src[1];
                    ((s16 *)pp)[1] += 0x1E;
                    SceneMovingParticle_Create(pp, vel2, 0x3C, 1);
                }
            }
            PL_AT(u16, 0x45A)++;
            if (PL_AT(u16, 0x45A) > 0x19) {
                if (gUnk_0200051C != 0 && gUnk_02000524 == 0 && PL_AT(u16, 0x430) != 0) {
                    gUnk_02000538 = sub_0814B078(p, 1000000, 0);
                    if (gUnk_02000538 != 0) {
                        PL_AT(u16, 0xA9E) += 0x32;
                        if (PL_AT(u16, 0xA9E) > 0xC8)
                            PL_AT(u16, 0xA9E) = 0xC8;
                    phase1_exit:
                        Player_SetActionPhase(p, 1);
                        break;
                    }
                }
                Player_SetActionPhase(p, 6);
                break;
            }
            if (*(u16 *)(PL_AT(u8 *, 0x348) + 2) & 2)
                gUnk_0200051C = 1;
            if (PL_AT(u8, 0x45F) == 1 && (*(u16 *)(PL_AT(u8 *, 0x348) + 2) & 1))
                gUnk_0200051C = 1;
            break;

        case 3:
            if (PL_AT(u16, 0x45A) == 0) {
                PL_AT(u8, 0xAA0) = ((PL_AT(u8, 0x2D) - 0x20) & ~0x3F) + 0x40;
                Sound_PlaySong(0x4FC);
                Sound_PlaySong(0x4FA);
            }
            cnt = PL_AT(u16, 0x45A);
            if (cnt > 5) {
                PL_AT(u8, 0xAA0) += 0x12 + (cnt >> 1);
                m = (cnt + 0x4B) * 2;
            } else if (cnt == 5) {
                PL_AT(u8, 0xAA0) += 0x40;
                m = (cnt >> 1) + 0x14;
            } else {
                m = (cnt >> 1) + 0x28;
            }
            pos[0] = Player_0814B1F8_Fix(gUnk_086149C4[(PL_AT(u8, 0xAA0) + 0x40) & 0xFF] * m);
            pos[1] = 0;
            pos[2] = Player_0814B1F8_Fix(gUnk_086149C4[PL_AT(u8, 0xAA0)] * m);
            PL_AT(u16, 0x10C) += pos[0];
            PL_AT(u16, 0x110) += pos[2];
            PL_AT(u16, 0x10C) += PL_AT(u16, 0x490);
            PL_AT(u16, 0x110) += PL_AT(u16, 0x494);
            ParticlePool_EmitTriple(PL_AT(void *, 0xA88), (u8 *)p + 0x10C,
                                    (PL_AT(u8, 0xAA0) + 0xA0) & 0xFF, 0);
            Player_SetFacingFromAngle(p, PL_AT(u8, 0xAA0));
            PLAYER_START_ANIM(0x1A2, 0x1A7, 1, 1);
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            if ((PL_AT(u16, 0x45A) & 3) == 0)
                TemporaryAnimationPool_CreateEntry((u8 *)p + 0xEC, 0x10, 0x200, PL_AT(u16, 0xAB0),
                                                   0);
            PlayerCue_InitAttackCollider(p, (u8 *)p + 0x10C, 1, 0x20);
            PL_AT(u16, 0x45A)++;
            if (PL_AT(u16, 0x45A) > 0x1D)
                Player_SetActionPhase(p, 4);
            break;

        case 4:
            if (PL_AT(u16, 0x45A) == 0) {
                PL_AT(u8, 0xAA0) = ((PL_AT(u8, 0x2D) - 0x20) & ~0x3F) + 0x40;
                ang = (PL_AT(u8, 0xAA0) - 0x80) & 0xFF;
                PL_AT(u16, 0x10C) = Player_0814B1F8_Fix(gUnk_086149C4[(ang + 0x40) & 0xFF] << 8);
                PL_AT(u16, 0x10E) = 0;
                PL_AT(u16, 0x110) = Player_0814B1F8_Fix(gUnk_086149C4[ang] * 0x100);
                PL_AT(u16, 0x10C) += PL_AT(u16, 0x30);
                PL_AT(u16, 0x10E) += PL_AT(u16, 0x32);
                PL_AT(u16, 0x110) += PL_AT(u16, 0x34);
            }
            dx = PL_AT(s16, 0x30) - PL_AT(s16, 0x10C);
            dy = PL_AT(s16, 0x34) - PL_AT(s16, 0x110);
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                PL_AT(u32, 0x10C) = PL_AT(u32, 0x30);
                PL_AT(u32, 0x110) = PL_AT(u32, 0x34);
                Player_SetActionPhase(p, 7);
            } else {
                PL_AT(u16, 0x10C) += Player_0814B1F8_Fix(gUnk_086149C4[(ang + 0x40) & 0xFF] * 20);
                PL_AT(u16, 0x110) += Player_0814B1F8_Fix(gUnk_086149C4[ang & 0xFF] * 20);
                PL_AT(u16, 0x45A)++;
                if (PL_AT(u16, 0x45A) > 0x59)
                    Player_SetActionPhase(p, 7);
            }
            if (PL_AT(u16, 0x45A) == 0)
                PLAYER_START_ANIM(0x1A4, 0x1A9, 2, 0);
            goto step_and_count;

        case 5:
            PLAYER_START_ANIM(0x1A1, 0x1A6, 1, 0);
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            Player_UpdateFacingSide(p);
            PL_AT(u16, 0x45A)++;
            if (PL_AT(u16, 0x45A) > 0x1F)
                ActorSequence_Begin(p, 0, 0);
            PL_AT(u32, 0x10C) = PL_AT(u32, 0x30);
            PL_AT(u32, 0x110) = PL_AT(u32, 0x34);
            PL_AT(u32, 0x20) |= 0x4000;
            break;

        case 6:
            dx = PL_AT(s16, 0x30) - PL_AT(s16, 0x10C);
            dy = PL_AT(s16, 0x34) - PL_AT(s16, 0x110);
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                PL_AT(u32, 0x10C) = PL_AT(u32, 0x30);
                PL_AT(u32, 0x110) = PL_AT(u32, 0x34);
                Player_SetActionPhase(p, 7);
            } else {
                PL_AT(u16, 0x10C) += Player_0814B1F8_Fix(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                PL_AT(u16, 0x110) += Player_0814B1F8_Fix(gUnk_086149C4[ang & 0xFF] * 0x32);
                PL_AT(u16, 0x45A)++;
                if (PL_AT(u16, 0x45A) > 0x59)
                    Player_SetActionPhase(p, 7);
            }
            if (PL_AT(u16, 0x45A) == 0)
                PLAYER_START_ANIM(0x1A4, 0x1A9, 2, 0);
        step_and_count:
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            PL_AT(u16, 0x45A)++;
            break;

        case 7:
            if (PL_AT(u16, 0x45A) == 0)
                PLAYER_START_ANIM(0x1A4, 0x1A9, 1, 0);
            Anim_Step((u8 *)p + 0xEC, (u8 *)p + 0x6C);
            if (PL_AT(u16, 0x45A) > 10)
                ActorSequence_Begin(p, 0, 0);
            PL_AT(u16, 0x45A)++;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814B1F8);
#endif
