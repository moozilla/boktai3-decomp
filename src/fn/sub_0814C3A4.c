#include "global.h"

struct C3A4Pos {
    s16 x, y, z;
};
struct C3A4Body {
    u8 unknown0[12];
    struct C3A4Pos pos;
};
struct C3A4Target {
    struct C3A4Body *body;
    u32 flags, unknown8;
    struct C3A4Target *next;
};
extern struct C3A4Target *gUnk_0200054C;
extern u16 gUnk_02000510;
extern u16 gUnk_02000530;
extern u16 gUnk_02000594;
extern u32 gUnk_030053F4;
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000710;

s32 Player_ConsumeStateEntry(u8 *);
void Sound_PlaySong(u32);
void Player_SetPendingCallback(u8 *, void *);
void Player_OnGrabMotionExit(u8 *);
void *ParticlePool_Create(void);
void WirelessGame_ResetCyclingItemLatch(void);
void Player_EmitQueuedEffects(u8 *);
void Player_NotifyActionCategories(u8 *);
void Player_AddControlFlags(u8 *, u32);
void Player_SetActionPhase(u8 *, u8);
s32 ActorVariant_GetTableValue(u32, u32);
s32 PlayerAnimation_SelectSecondaryKeyframes(u8 *, s32, s32);
struct C3A4Target *sub_0814C248(u8 *);
s32 Math_Atan2(s32, s32);
s32 sub_08249244(void *, s32, s32, u32);
void Player_UpdateFacingSide(u8 *);
void ParticlePool_EmitTriple(void *, void *, u32, u32);
void SceneParticle_CreateDefaultPriority(void *, void *, u32, u32, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, u32);
void TemporaryAnimationPool_CreateEntry(void *, u32, u32, u32, u32);
void PlayerCue_InitAttackCollider(u8 *, void *, u32, u32);
void ActorSequence_Begin(u8 *, u32, u32);
u32 Sqrt(u32);
s32 Div(s32, s32);
s32 Player_ScaleLevelValue(s32, s32);

#define C3A4_U16(p, off) (*(u16 *)((p) + (off)))
#define C3A4_S16(p, off) (*(s16 *)((p) + (off)))
#define C3A4_U32(p, off) (*(u32 *)((p) + (off)))

static inline s32 C3A4_Fx12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 C3A4_Sin(u32 angle)
{
    return gUnk_086149C4[angle & 0xFF];
}
/* Selects the facing pair at +0x3A2/+0x3A3 from a quarter index. */
static inline void C3A4_SetFacing(u8 *p, s32 quarter)
{
    if (quarter > 1) {
        p[0x3A2] = 3 - quarter;
        p[0x3A3] = 1;
    } else {
        p[0x3A2] = quarter;
        p[0x3A3] = 0;
    }
}
static inline void C3A4_Hit(struct C3A4Target *t, s32 a, s32 b)
{
    sub_08249244(t->body, a, b, t->unknown8);
}
static inline void C3A4_CopyPosition(u8 *p)
{
    C3A4_U32(p, 0x10C) = C3A4_U32(p, 0x30);
    C3A4_U32(p, 0x110) = C3A4_U32(p, 0x34);
}

#ifdef NONMATCHING
// fuzzy: 53.9%
// scope shape 455 (differences once low registers are set aside)
// confidence=low: Player grab/throw action update; entry setup, then an 8-phase switch (reach, pull, throw, release, ...).
void sub_0814C3A4(u8 *p)
{
    if (Player_ConsumeStateEntry(p)) {
        gUnk_02000510 = 0;
        gUnk_02000530 = 0;
        gUnk_02000594 = 0;
        C3A4_U16(p, 0x274) = 0;
        gUnk_0200054C = 0;
        C3A4_U16(p, 0xAD6) = 0x16F;
        C3A4_U16(p, 0xAD8) = 0x20;
        Sound_PlaySong(0x39E);
        C3A4_U32(p, 0xF4) &= ~1;
        C3A4_U32(p, 0x10C) = C3A4_U32(p, 0xAC);
        C3A4_U32(p, 0x110) = C3A4_U32(p, 0xB0);
        C3A4_U16(p, 0x126) = C3A4_U16(p, 0xC6);
        C3A4_U32(p, 0x134) = C3A4_U32(p, 0xD4);
        C3A4_U32(p, 0xA8C) = C3A4_U32(p, 0x30);
        C3A4_U32(p, 0xA90) = C3A4_U32(p, 0x34);
        Player_SetPendingCallback(p, Player_OnGrabMotionExit);
        if (C3A4_U32(p, 0xA88) == 0)
            C3A4_U32(p, 0xA88) = (u32)ParticlePool_Create();
        if (gUnk_030053F4 & 0x1000)
            WirelessGame_ResetCyclingItemLatch();
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }
    Player_AddControlFlags(p, 9);
    C3A4_U32(p, 0x20) = (C3A4_U32(p, 0x20) & ~0x4000) | 0x60000;
    switch (p[0x458]) {
        case 0: {
            u32 v = C3A4_U16(p, 0x45A);
            if ((v & 7) < 7 - (v >> 3))
                PlayerAnimation_SelectSecondaryKeyframes(p, ActorVariant_GetTableValue(p[0x418], 0),
                                                         0x40);
            else
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            C3A4_CopyPosition(p);
            gUnk_0200054C = sub_0814C248(p);
            if (gUnk_0200054C == 0) {
                C3A4_U32(p, 0x20) |= 0x4000;
            } else {
                Player_SetActionPhase(p, 1);
                Sound_PlaySong(0x53C);
            }
            C3A4_U16(p, 0x45A)++;
            if (C3A4_U16(p, 0x45A) <= 0x1D)
                return;
            gUnk_0200054C = sub_0814C248(p);
            if (gUnk_0200054C)
                return;
            Player_SetActionPhase(p, 5);
            return;
        }
        case 1: {
            struct C3A4Target *t = gUnk_0200054C;
            s32 dx = t->body->pos.x - C3A4_S16(p, 0x10C);
            s32 dy;
            s32 dz;
            s32 angle;
            t = gUnk_0200054C;
            dy = t->body->pos.y - C3A4_S16(p, 0x10E);
            t = gUnk_0200054C;
            dz = t->body->pos.z - C3A4_S16(p, 0x110);
            angle = Math_Atan2(dx, dz);
            if (dx * dx + dz * dz + dy * dy > 0x8FF) {
                s32 quarter;
                if (dy > 0x18)
                    C3A4_U16(p, 0x10E) += 0x18;
                else if (dy < -0x18)
                    C3A4_U16(p, 0x10E) -= 0x18;
                C3A4_U16(p, 0x10C) += C3A4_Fx12(C3A4_Sin(angle + 0x40) * 24);
                C3A4_U16(p, 0x110) += C3A4_Fx12(C3A4_Sin(angle) * 24);
                p[0x3A4] = ((((angle + 0x20) & 0xFF) >> 6) * 2 + 3) & 7;
                quarter = (((angle + 0x60) & 0xFF) >> 6);
                C3A4_SetFacing(p, quarter);
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
                C3A4_U16(p, 0x45A)++;
                if (C3A4_U16(p, 0x45A) > 0x59)
                    Player_SetActionPhase(p, 6);
            } else {
                t = gUnk_0200054C;
                if (t->flags & 1) {
                    gUnk_0200054C = 0;
                    Player_SetActionPhase(p, 5);
                } else {
                    s32 r = sub_08249244(t->body, 0, 0, t->unknown8);
                    C3A4_U32(gUnk_02000710, 0xC00) += 8;
                    gUnk_02000594 = 1;
                    if (r == 0) {
                        gUnk_0200054C = 0;
                        Player_SetActionPhase(p, 5);
                    } else {
                        s32 base;
                        s32 quarter;
                        p[0x3A4] = ((((angle + 0x20) & 0xFF) >> 6) * 2 + 3) & 7;
                        if (!(gUnk_0200054C->flags & 0x10))
                            ((u8 *)gUnk_0200054C->body)[9] = ((p[0x3A4] + 5) & 7) << 5;
                        base = ((angle + 0x60) & 0xFF) >> 6;
                        quarter = base;
                        C3A4_SetFacing(p, quarter);
                        PlayerAnimation_SelectSecondaryKeyframes(p, 0xF0, 0x40);
                        Player_SetActionPhase(p, 2);
                    }
                }
            }
            ParticlePool_EmitTriple((void *)C3A4_U32(p, 0xA88), p + 0x10C, (angle + 0x80) & 0xFF,
                                    1);
            return;
        }
        case 2: {
            struct C3A4Target *t = gUnk_0200054C;
            s32 r;
            s32 damage;
            if (t->flags & 1) {
                if (t->flags & 0x10)
                    C3A4_Hit(t, 1, 0x3C);
                else
                    C3A4_Hit(t, 1, 0x3C);
                gUnk_0200054C = 0;
                Player_UpdateFacingSide(p);
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
                Player_SetActionPhase(p, 6);
                return;
            }
            if (gUnk_02000510 <= 3 && C3A4_U16(p, 0x430)) {
                u32 v = *(u16 *)(*(u8 **)(p + 0x348) + 2);
                if ((v & 2) || ((v & 1) && p[0x45F] == 1))
                    gUnk_02000530 = 1;
            }
            r = PlayerAnimation_SelectSecondaryKeyframes(p, 0xF0, 0x40);
            if (C3A4_U16(p, 0x100) == 5 && C3A4_U16(p, 0xF8) == 0) {
                s32 amount;
                s32 base;
                struct C3A4Pos vec;
                if (gUnk_030053F4 & 0x1000)
                    amount = 30;
                else
                    amount = Player_ScaleLevelValue(2, 0x60);
                t = gUnk_0200054C;
                if (t->flags & 0x10)
                    r = sub_08249244(t->body, 0, amount, t->unknown8);
                else
                    r = sub_08249244(t->body, 0, amount, t->unknown8);
                if (r != 1) {
                    gUnk_0200054C = 0;
                    Player_UpdateFacingSide(p);
                    Player_SetActionPhase(p, 6);
                    return;
                }
                Sound_PlaySong(0x39F);
                base = ((p[0x3A4] + 5) & 7) << 5;
                vec.x = C3A4_Fx12(C3A4_Sin(base + 0x40) * 64);
                vec.y = 0xE6;
                vec.z = C3A4_Fx12(C3A4_Sin(base) * 64);
                SceneParticle_CreateDefaultPriority(p + 0x10C, &vec, 0, 0, 1);
                damage = amount;
                if (gUnk_030053F4 & 0x1000)
                    damage = 10;
                C3A4_U16(p, 0x424) += damage;
                if (C3A4_U16(p, 0x424) >= C3A4_U16(p, 0x426)) {
                    damage = C3A4_U16(p, 0x426) - C3A4_U16(p, 0x424);
                    C3A4_U16(p, 0x424) = C3A4_U16(p, 0x426);
                }
                if (damage > 0)
                    DamageNumberDisplay_Show(p + 0x230, damage, 1, p + 0x10C, p + 0xAE8, 0);
                gUnk_02000510++;
                return;
            }
            if (r == 0)
                return;
            if (gUnk_02000530) {
                Player_SetActionPhase(p, 2);
            } else {
                Player_AddControlFlags(p, 8);
                t = gUnk_0200054C;
                if (t->flags & 0x10)
                    C3A4_Hit(t, 1, 0x3C);
                else
                    C3A4_Hit(t, 1, 0x3C);
                gUnk_0200054C = 0;
                Player_SetActionPhase(p, 6);
            }
            gUnk_02000530 = 0;
            return;
        }
        case 3: {
            u32 h = C3A4_U16(p, 0x45A);
            s32 m;
            struct C3A4Pos vec;
            if (h == 0) {
                p[0xAA0] = p[0x2D];
                Sound_PlaySong(0x4F6);
                Sound_PlaySong(0x576);
            }
            h = C3A4_U16(p, 0x45A);
            if (h > 5) {
                p[0xAA0] += 0x12 + (h >> 1);
                m = (C3A4_U16(p, 0x45A) + 0x4B) * 2;
            } else if (h == 5) {
                p[0xAA0] += 0x40;
                m = (C3A4_U16(p, 0x45A) >> 1) + 0x14;
            } else {
                m = (h >> 1) + 0x28;
            }
            vec.x = C3A4_Fx12(C3A4_Sin(p[0xAA0] + 0x40) * m);
            vec.y = 0;
            vec.z = C3A4_Fx12(C3A4_Sin(p[0xAA0]) * m);
            C3A4_U16(p, 0x10C) += vec.x;
            C3A4_U16(p, 0x110) += vec.z;
            C3A4_U16(p, 0x10C) += C3A4_U16(p, 0x490);
            C3A4_U16(p, 0x110) += C3A4_U16(p, 0x494);
            ParticlePool_EmitTriple((void *)C3A4_U32(p, 0xA88), p + 0x10C, (p[0xAA0] + 0xA0) & 0xFF,
                                    1);
            p[0x3A2] = p[0xAA0];
            C3A4_SetFacing(p, ((p[0xAA0] + 0x60) >> 6) & 3);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
            if ((C3A4_U16(p, 0x45A) & 3) == 0)
                TemporaryAnimationPool_CreateEntry(p + 0xEC, 0x10, 0x200, C3A4_U16(p, 0xAB0), 0);
            PlayerCue_InitAttackCollider(p, p + 0x10C, 2, 0x20);
            C3A4_U16(p, 0x45A)++;
            if (C3A4_U16(p, 0x45A) > 0x23)
                Player_SetActionPhase(p, 4);
            return;
        }
        case 4:
            ActorSequence_Begin(p, 0, 0);
            return;
        case 5:
            C3A4_U16(p, 0xAD6) = 0x16F;
            C3A4_U16(p, 0xAD8) = 0x20;
            Player_UpdateFacingSide(p);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            C3A4_U16(p, 0x45A)++;
            if (C3A4_U16(p, 0x45A) > 0x1F)
                ActorSequence_Begin(p, 0, 0);
            C3A4_CopyPosition(p);
            C3A4_U32(p, 0x20) |= 0x4000;
            return;
        case 6: {
            s32 angle;
            Player_AddControlFlags(p, 8);
            if (C3A4_U16(p, 0x45A) == 0) {
                s32 dx = C3A4_S16(p, 0xA8C) - C3A4_S16(p, 0x10C);
                s32 dz = C3A4_S16(p, 0xA90) - C3A4_S16(p, 0x110);
                s32 speed = Div((u32)((u16)Sqrt(dx * dx + dz * dz)) << 4, 12);
                if (speed >= 0)
                    speed >>= 4;
                else
                    speed = -((-speed) >> 4);
                C3A4_U16(p, 0xA9E) = speed;
                C3A4_U16(p, 0xAD6) = 0x16F;
                C3A4_U16(p, 0xAD8) = 0x20;
            }
            Player_UpdateFacingSide(p);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            angle = Math_Atan2(C3A4_S16(p, 0xA8C) - C3A4_S16(p, 0x10C),
                               C3A4_S16(p, 0xA90) - C3A4_S16(p, 0x110));
            C3A4_U16(p, 0x10C) += C3A4_Fx12(C3A4_Sin(angle + 0x40) * C3A4_U16(p, 0xA9E));
            C3A4_U16(p, 0x110) += C3A4_Fx12(C3A4_Sin(angle) * C3A4_U16(p, 0xA9E));
            C3A4_U16(p, 0x4E8) = C3A4_Fx12(C3A4_Sin((C3A4_U16(p, 0x45A) + 4) << 3) << 6);
            C3A4_U16(p, 0x45A)++;
            if (C3A4_U16(p, 0x45A) > 0xB) {
                C3A4_U16(p, 0x4E8) = 0;
                Sound_PlaySong(0x1B0);
                Player_SetActionPhase(p, 7);
            }
            return;
        }
        case 7:
            if (PlayerAnimation_SelectSecondaryKeyframes(p, 0xDA, 0x40))
                ActorSequence_Begin(p, 0, 0);
            return;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814C3A4);
#endif
