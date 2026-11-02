#include "player.h"
// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_actor.h"
extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
u32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_UpdateAttackFacing(void *);
void Sound_PlaySong(u32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 ActorResponse_Apply(void *, s32);
s32 Player_SpawnSelectedAttackProjectile(void *);
void sub_081DAD94(s32, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_GetAttackEffectType(u32);
#define HELD       (p->inputKeys[0] & 2)
#define PRESSED    (p->inputKeys[1] & 2)
#define ANIMATE(n) Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, n), 64, 1)

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void SetChargeParameter(struct Player *p)
{
    u16 *target = &p->actionEighteenParameter;
    u32 value = 0x16E;
    *target = value;
}
void Player_ReleaseActionEffectOnExit(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
void Player_UpdateAttachedEntryPosition(void *);
s32 Task4E01_GetKind7Value(s32);
static inline u32 bonus(struct Player *p, u32 mask)
{
    u32 *field;
    u32 bits;
    if (!p)
        return 0;
    field = &p->statBonusFlags;
    bits = mask;
    return *field & bits;
} // confidence=low: attached task tiers and buffered restart define this charge handler.
void Player_UpdateAttachedAttackCharge(struct Player *p)
{
    struct ContactPackedVector origin;
    struct ContactPackedVector spread;
    u16 *t;
    u16 *r;
    if (Player_ConsumeStateEntry(p)) {
        u16 *first = &p->statModifierContext.contactTimer;
        s32 zero = 0;
        *first = zero;
        p->statModifierContext.attachedEntry = --zero;
        Player_SetPendingCallback(p, Player_ReleaseActionEffectOnExit);
    }
    switch (p->tetherPhase) {
        case 0:
            if (ANIMATE(0x26)) {
                u16 *duration = &p->statModifierContext.chargeDuration;
                u32 zero = 0;
                *duration = 60;
                p->contactCounted = zero;
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            t = &p->tetherFrame;
            if (!*t)
                Sound_PlaySong(0x1F7);
            if (HELD) {
                Player_UpdateAttackFacing(p);
                ANIMATE(0x27);
            }
            if (++*t > 6) {
                {
                    u8 *chargeTier = &p->statModifierContext.chargeTier;
                    *chargeTier = 0;
                }
                p->contactMagnitude = 0;
                if (p->currentStatB <= 2) {
                    Sound_PlaySong(0xD5);
                    goto release;
                }
                if (bonus(p, 0x10000000))
                    Player_SetActionPhase(p, 4);
                else
                    Player_SetActionPhase(p, 2);
                if (!(bonus(p, 0x10000000)))
                    Player_SetActionPhase(p, 4);
                else {
                    if (!Player_SpawnSelectedAttackProjectile(p))
                        goto release;
                    Player_SetActionPhase(p, 2);
                }
            }
            goto attached;
        case 2:
            if (++p->tetherFrame == 6)
                Sound_PlaySong(0x1F4);
            if (!HELD)
                goto fire;
            if (Task4E01_GetKind7Value(p->statModifierContext.attachedEntry) > 1) {
                {
                    u8 *chargeTier = &p->statModifierContext.chargeTier;
                    *chargeTier = 1;
                }
                p->contactMagnitude = 30;
                SetChargeParameter(p);
                p->actionEighteenDuration = 36;
                Player_SetActionPhase(p, 3);
            }
            goto face;
        case 3: {
            register u32 inputOffset asm("r2") = (u32) & ((struct Player *)0)->inputKeys;
            register const u16 **input asm("r0");
            asm("" : "+r"(inputOffset));
            input = (const u16 **)((u8 *)p + inputOffset);
            if (!(**input & 2))
                goto fire;
            if (Task4E01_GetKind7Value(p->statModifierContext.attachedEntry) > 2) {
                {
                    u8 *chargeTier = &p->statModifierContext.chargeTier;
                    *chargeTier = 1;
                }
                p->contactMagnitude = 30;
                SetChargeParameter(p);
                p->actionEighteenDuration = 36;
                Sound_PlaySong(0x1F5);
                Player_SetActionPhase(p, 4);
            }
            goto face;
        }
        case 4: {
            register u32 inputOffset asm("r2") = (u32) & ((struct Player *)0)->inputKeys;
            register const u16 **input asm("r0");
            asm("" : "+r"(inputOffset));
            input = (const u16 **)((u8 *)p + inputOffset);
            if (!(**input & 2)) {
            fire:
                if (Player_SpawnSelectedAttackProjectile(p))
                    Sound_PlaySong(0x1F6);
            release:
                Player_SetActionPhase(p, 5);
            } else {
            face:
                Player_UpdateAttackFacing(p);
                ANIMATE(0x27);
            }
        attached:
            Player_UpdateAttachedEntryPosition(p);
            break;
        }
        case 5:
            ANIMATE(0x28);
            if (PRESSED)
                p->statModifierContext.bufferedAttack = 1;
            r = &p->contactMagnitude;
            if (*r) {
                s32 ang, n;
                if (*r > 5) {
                    origin.x = 0;
                    origin.y = 0;
                    origin.z = 0;
                    spread.x = 32;
                    spread.y = 32;
                    spread.z = 32;
                    SceneParticleBurst_CreateWhilePaused(&p->position, &origin, &spread, 1, 3, 4);
                }
                ang = ((p->facing + 5) & 7) << 5;
                n = *r;
                p->velocityX = -scale(gUnk_086149C4[(ang + 64) & 255] * n);
                p->velocityZ = -scale(p->contactMagnitude * gUnk_086149C4[ang]);
                p->contactMagnitude = p->contactMagnitude * 3 >> 2;
            } else if (++p->tetherFrame > 4)
                Player_SetActionPhase(p, 6);
            break;
        case 6: {
            u32 pending;
            ANIMATE(0x27);
            if (PRESSED)
                p->statModifierContext.bufferedAttack = 1;
            pending = p->statModifierContext.bufferedAttack;
            if (pending) {
                p->statModifierContext.attachedEntry = -1;
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else if (++p->tetherFrame > 14) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        }
        case 7:
            ANIMATE(0x26);
            if (++p->tetherFrame > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
    p->actionVariant = 4;
}
