#include "global.h"

#define P8(o)   (*(u8 *)((u8 *)p + (o)))
#define P16(o)  (*(u16 *)((u8 *)p + (o)))
#define PS16(o) (*(s16 *)((u8 *)p + (o)))
#define P32(o)  (*(u32 *)((u8 *)p + (o)))

extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern u32 gUnk_0200051C, gUnk_02000524;

s32 Player_ConsumeStateEntry(void *);
u32 ParticlePool_Create(void);
void Player_SetPendingCallback(void *, void *);
void Player_OnTargetMotionExit(void);
void Player_EmitQueuedEffects(void *);
void Player_NotifyActionCategories(void *);
void Player_AddControlFlags(void *, u32);
s32 Math_Atan2(s32, s32);
void Player_SetFacingFromAngle(void *, s32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Player_SetActionPhase(void *, s32);
void ParticlePool_EmitTriple(u32, void *, u32, u32);
void Sound_PlaySong(u32);
void SceneMovingParticle_Create(void *, void *, s32, s32);
void PlayerCue_InitAttackCollider(void *, void *, u32, u32);
s32 Div(s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);

struct sub_081561EC_Vec {
    s16 x;
    s16 y;
    s16 z;
};

static inline s32 sub_081561EC_Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline u32 sub_081561EC_Rand(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return gUnk_0203B400[gUnk_03005308];
}

static inline void sub_081561EC_Anim(u8 *p, u32 frame, u32 a, u32 b, u32 mode)
{
    void *anim = p + 0xEC;
    void *data = p + 0x6C;
    if (frame == 0) {
        if (P8(0x3A2) == 0)
            Anim_SetKeyframes(anim, data, a, mode, 0);
        else
            Anim_SetKeyframes(anim, data, b, mode, 0);
        if (P8(0x3A3) != 0)
            P32(0xEC + 8) |= 4;
        else
            P32(0xEC + 8) &= ~4;
    }
    Anim_Step(anim, data);
}

static inline void sub_081561EC_Spark(u8 *p)
{
    struct sub_081561EC_Vec vel;
    struct sub_081561EC_Vec pos;
    u32 a = sub_081561EC_Rand() & 0xFF;
    s32 m = (sub_081561EC_Rand() & 0x1F) - 0x10;
    vel.x = sub_081561EC_Scale(gUnk_086149C4[(a + 0x40) & 0xFF] * m);
    vel.y = 0;
    vel.z = sub_081561EC_Scale(gUnk_086149C4[a] * m);
    vel.y = ((sub_081561EC_Rand() & 0x3F) << 1) - 0x30;
    *(u32 *)&pos = P32(0x590);
    *((u32 *)&pos + 1) = P32(0x594);
    pos.y += 0x1E;
    SceneMovingParticle_Create(&pos, &vel, 60, 1);
}

#ifdef NONMATCHING
// fuzzy: 54.3%
// scope shape 332 (differences once low registers are set aside)
// confidence=low: Player action "dash to target": 5-phase state machine (approach, run, strike, return, stand)
void sub_081561EC(u8 *p)
{
    s32 ang, dx, dy, ph;
    u32 frame;

    if (Player_ConsumeStateEntry(p)) {
        if (P32(0xA88) == 0)
            P32(0xA88) = ParticlePool_Create();
        P16(0xA9E) = 0x32;
        gUnk_0200051C = 0;
        gUnk_02000524 = 0;
        *(u32 *)(p + 0xA8C) = *(u32 *)(p + 0x30);
        *(u32 *)(p + 0xA90) = *(u32 *)(p + 0x34);
        *(u32 *)(p + 0x10C) = *(u32 *)(p + 0xB0);
        *(u32 *)(p + 0x110) = *(u32 *)(p + 0xAC);
        P32(0xEC + 8) &= ~1;
        Player_SetPendingCallback(p, Player_OnTargetMotionExit);
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }
    P16(0x126) = P16(0xC6);
    P32(0x134) = P32(0xD4);
    Player_AddControlFlags(p, 9);
    P32(0x20) = (P32(0x20) & ~0x4000) | 0x60000;

    switch (P8(0x458)) {
        case 0:
            frame = P16(0x45A);
            if (frame == 0) {
                Player_SetFacingFromAngle(
                    p, Math_Atan2(PS16(0x590) - PS16(0x10C), PS16(0x594) - PS16(0x110)));
            }
            sub_081561EC_Anim(p, frame, 0x1A1, 0x1A6, 1);
            P16(0x45A)++;
            if (P16(0x45A) > 10)
                Player_SetActionPhase(p, 1);
            break;
        case 1:
            dx = PS16(0x590) - PS16(0x10C);
            dy = PS16(0x594) - PS16(0x110);
            ang = Math_Atan2(dx, dy);
            ParticlePool_EmitTriple(P32(0xA88), p + 0x10C, (ang + 0x80) & 0xFF, 0);
            frame = P16(0x45A);
            if (frame == 0) {
                Player_SetFacingFromAngle(p, ang);
                Sound_PlaySong(0x4F5);
            }
            sub_081561EC_Anim(p, frame, 0x1A2, 0x1A7, 1);
            if (dx * dx + dy * dy < P16(0xA9E) * P16(0xA9E)) {
                P8(0x3A4) = (((((ang + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                P16(0x10E) = P16(0x592);
                Player_SetFacingFromAngle(p, ang);
                Player_SetActionPhase(p, 2);
                sub_081561EC_Spark(p);
            } else {
                P16(0x10C) += sub_081561EC_Scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * P16(0xA9E));
                P16(0x110) += sub_081561EC_Scale(gUnk_086149C4[ang & 0xFF] * P16(0xA9E));
                P8(0x3A4) = (((((ang + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                if (Div(P16(0xA9C), 2) < (s32)P16(0x45A)) {
                    P16(0x10E) -= 0x50;
                    if (PS16(0x10E) < PS16(0x592))
                        P16(0x10E) = P16(0x592);
                } else {
                    P16(0x10E) += 0x50;
                    if (PS16(0x10E) > PS16(0x592) + 1000)
                        P16(0x10E) = P16(0x592) + 1000;
                }
                ph = ((ang + 0x60) & 0xFF) >> 6;
                if (ph > 1) {
                    P8(0x3A2) = 3 - ph;
                    P8(0x3A3) = 1;
                } else {
                    P8(0x3A2) = ph;
                    P8(0x3A3) = 0;
                }
                P16(0x45A)++;
                if (P16(0x45A) > 0x59)
                    Player_SetActionPhase(p, 3);
            }
            if (*(u16 *)(P32(0x348) + 2) & 2)
                gUnk_02000524 = 1;
            PlayerCue_InitAttackCollider(p, p + 0x10C, 1, 0x20);
            break;
        case 2:
            if (P16(0x45A) == 0) {
                Sound_PlaySong(0x4FB);
                dx = PS16(0x590) - PS16(0x10C);
                dy = PS16(0x594) - PS16(0x110);
                if (dx * dx + dy * dy > 0x9C4) {
                    ang = Math_Atan2(dx, dy);
                    Player_SetFacingFromAngle(p, ang);
                    P16(0x10C) =
                        P16(0x590) + sub_081561EC_Scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                    P16(0x110) = P16(0x594) + sub_081561EC_Scale(gUnk_086149C4[ang & 0xFF] * 0x32);
                }
                gUnk_0200051C = 0;
            }
            if (P16(0x45A) == 5) {
                if (P8(0x3A2) == 0)
                    Anim_SetKeyframes(p + 0xEC, p + 0x6C, 0x1A3, 2, 0);
                else
                    Anim_SetKeyframes(p + 0xEC, p + 0x6C, 0x1A8, 2, 0);
                if (P8(0x3A3) != 0)
                    P32(0xEC + 8) |= 4;
                else
                    P32(0xEC + 8) &= ~4;
            }
            if ((P16(0x45A) & 1) == 0)
                sub_081561EC_Spark(p);
            P16(0x45A)++;
            if (P16(0x45A) > 0x19) {
                Player_SetActionPhase(p, 3);
                break;
            }
            if (*(u16 *)(P32(0x348) + 2) & 2)
                gUnk_0200051C = 1;
            break;
        case 3:
            dx = PS16(0x30) - PS16(0x10C);
            dy = PS16(0x34) - PS16(0x110);
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                *(u32 *)(p + 0x10C) = *(u32 *)(p + 0x30);
                *(u32 *)(p + 0x110) = *(u32 *)(p + 0x34);
                Player_SetActionPhase(p, 4);
            } else {
                P16(0x10C) += sub_081561EC_Scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                P16(0x110) += sub_081561EC_Scale(gUnk_086149C4[ang & 0xFF] * 0x32);
                P16(0x45A)++;
                if (P16(0x45A) > 0x59)
                    Player_SetActionPhase(p, 4);
            }
            sub_081561EC_Anim(p, P16(0x45A), 0x1A4, 0x1A9, 2);
            P16(0x45A)++;
            break;
        case 4:
            sub_081561EC_Anim(p, P16(0x45A), 0x1A4, 0x1A9, 1);
            if (P16(0x45A) > 10) {
                ActorSequence_Begin(p, 0, 0);
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            P16(0x45A)++;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081561EC);
#endif
