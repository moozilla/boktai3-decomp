#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "player/player.h"
#include "objects/scene_object.h"
#include "player/player_status_flags.h"
#include "player/player_slot_status.h"

struct PlayerGrabPos {
    s16 x, y, z;
};
struct PlayerGrabBody {
    u8 unknown0[12];
    struct PlayerGrabPos pos;
};
struct PlayerGrabTarget {
    struct PlayerGrabBody *body;
    u32 flags, unknown8;
    struct PlayerGrabTarget *next;
};
extern u16 gUnk_02000510;
extern u16 gUnk_02000530;
extern const s16 gUnk_086149C4[];

s32 Player_ConsumeStateEntry(struct Player *);
void Sound_PlaySong(u32);
void Player_SetPendingCallback(struct Player *, void *);
void Player_OnGrabMotionExit(struct Player *);
void *ParticlePool_Create(void);
void WirelessGame_ResetCyclingItemLatch(void);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void Player_SetActionPhase(struct Player *, u8);
s32 ActorVariant_GetTableValue(u32, u32);
s32 PlayerAnimation_SelectSecondaryKeyframes(struct Player *, s32, s32);
struct PlayerGrabTarget *Player_FindNearestVisibleTarget(struct Player *);
s32 Math_Atan2(s32, s32);
s32 sub_08249244(void *, s32, s32, u32);
void Player_UpdateFacingSide(struct Player *);
void ParticlePool_EmitTriple(void *, void *, u32, u32);
void SceneParticle_CreateDefaultPriority(void *, void *, u32, u32, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, u32);
void TemporaryAnimationPool_CreateEntry(void *, u32, u32, u32, u32);
void PlayerCue_InitAttackCollider(struct Player *, void *, u32, u32);
void ActorSequence_Begin(struct Player *, u32, u32);
u32 Sqrt(u32);
s32 Div(s32, s32);
s32 Player_ScaleLevelValue(s32, s32);

static inline u16 PlayerGrab_GetFrame(struct Player *p)
{
    return p->tetherFrame;
}

static inline s32 PlayerGrab_Fx12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 PlayerGrab_Fx4(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 4;
    else
        result = -((-value) >> 4);
    return result;
}

static inline s32 PlayerGrab_ReadSin(const s16 *table, u32 angle)
{
    return table[angle & 0xFF];
}
#define PlayerGrab_Sin(angle) PlayerGrab_ReadSin(gUnk_086149C4, (angle))
/* Selects the facing pair at +0x3A2/+0x3A3 from a quarter index. */
static inline void PlayerGrab_SetFacing(struct Player *p, s32 quarter)
{
    if (quarter > 1) {
        s32 folded = 3 - quarter;
        p->heading = folded;
        p->side = 1;
    } else {
        p->heading = quarter;
        p->side = 0;
    }
}
static inline void PlayerGrab_SetFacingAt(struct Player *p, u8 *heading, u16 input)
{
    s32 quarter = input;
    if (quarter > 1) {
        u8 folded = 3 - quarter;
        *heading = folded;
        p->side = 1;
    } else {
        *heading = quarter;
        p->side = 0;
    }
}
static inline s32 PlayerGrab_Hit(struct PlayerGrabTarget *t, s32 a, s32 b)
{
    return sub_08249244(t->body, a, b, t->unknown8);
}
static inline void PlayerGrab_CopyPosition(struct Player *p)
{
    p->secondaryAnimation.position = *(struct ContactPosition *)&p->position;
}

static inline void PlayerGrab_Polar(struct PlayerGrabPos *vec, u32 angle, s32 radius)
{
    vec->x = PlayerGrab_Fx12(PlayerGrab_Sin(angle + 0x40) * radius);
    vec->y = 0;
    vec->z = PlayerGrab_Fx12(PlayerGrab_Sin(angle) * radius);
}

static inline struct PlayerGrabPos *PlayerGrab_TargetPosition(struct PlayerGrabTarget *target)
{
    if (target->flags & 0x10)
        return &target->body->pos;
    else
        return &target->body->pos;
}

// confidence=low: Player grab/throw action update; entry setup, then an 8-phase switch (reach, pull, throw, release, ...).
void Player_UpdateGrabMotion(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        gUnk_02000510 = 0;
        gUnk_02000530 = 0;
        gPlayerNotificationPending = 0;
        {
            u32 clear = 0;
            p->positionResetValue = clear;
            (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = (void *)clear;
        }
        p->actionEighteenParameter = 0x16F;
        p->actionEighteenDuration = 0x20;
        Sound_PlaySong(0x39E);
        AnimSprite_Show(&p->secondaryAnimation);
        p->secondaryAnimation.position = p->animation.position;
        p->secondaryAnimation.paletteBank = p->animation.paletteBank;
        p->secondaryAnimation.palette = p->animation.palette;
        p->movementStart = p->position;
        Player_SetPendingCallback(p, Player_OnGrabMotionExit);
        if (p->unkA88[0] == 0)
            p->unkA88[0] = (u32)ParticlePool_Create();
        if (gEngineFlags & 0x1000)
            WirelessGame_ResetCyclingItemLatch();
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }
    Player_AddControlFlags(p, 9);
    p->statusFlags = Player_ReadStatusBits(&p->statusFlags, ~0x4000) | 0x60000;
    switch (p->tetherPhase) {
        case 0: {
            if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 3))
                PlayerAnimation_SelectSecondaryKeyframes(
                    p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
            else
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            PlayerGrab_CopyPosition(p);
            (*(struct PlayerGrabTarget **)&gPlayerPendingActor) =
                Player_FindNearestVisibleTarget(p);
            if ((*(struct PlayerGrabTarget **)&gPlayerPendingActor) == 0) {
                Player_AddStatusFlags(p, 0x4000);
            } else {
                Player_SetActionPhase(p, 1);
                Sound_PlaySong(0x53C);
            }
            p->tetherFrame++;
            if (p->tetherFrame <= 0x1D)
                return;
            (*(struct PlayerGrabTarget **)&gPlayerPendingActor) =
                Player_FindNearestVisibleTarget(p);
            if ((*(struct PlayerGrabTarget **)&gPlayerPendingActor))
                return;
            Player_SetActionPhase(p, 5);
            return;
        }
        case 1: {
            struct PlayerGrabTarget *t;
            s32 dx = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->x -
                     p->secondaryAnimation.position.x;
            s32 dy;
            s32 dz;
            s32 angle;
            dy = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->y -
                 p->secondaryAnimation.position.y;
            dz = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->z -
                 p->secondaryAnimation.position.z;
            angle = Math_Atan2(dx, dz);
            if (dx * dx + dz * dz + dy * dy <= 0x8FF) {
                t = (*(struct PlayerGrabTarget **)&gPlayerPendingActor);
                if (t->flags & 1) {
                    (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = 0;
                    Player_SetActionPhase(p, 5);
                } else {
                    s32 r;
                    if (t->flags & 0x10)
                        r = PlayerGrab_Hit(t, 0, 0);
                    else
                        r = PlayerGrab_Hit(t, 0, 0);
                    gGameState->unk600 += 8;
                    gPlayerNotificationPending = 1;
                    if (r == 0) {
                        (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = 0;
                        Player_SetActionPhase(p, 5);
                    } else {
                        p->facing = (((angle + 0x20) & 0xFF) >> 6) * 2 + 3 & 7;
                        if (!((*(struct PlayerGrabTarget **)&gPlayerPendingActor)->flags & 0x10))
                            ((u8 *)(*(struct PlayerGrabTarget **)&gPlayerPendingActor)->body)[9] =
                                ((p->facing + 5) & 7) << 5;
                        PlayerGrab_SetFacing(p, ((angle + 0x60) & 0xFF) >> 6);
                        PlayerAnimation_SelectSecondaryKeyframes(p, 0xF0, 0x40);
                        Player_SetActionPhase(p, 2);
                    }
                }
            } else {
                if (dy > 0x18)
                    p->secondaryAnimation.position.y += 0x18;
                else if (dy < -0x18)
                    p->secondaryAnimation.position.y -= 0x18;
                p->secondaryAnimation.position.x +=
                    PlayerGrab_Fx12(PlayerGrab_Sin(angle + 0x40) * 24);
                p->secondaryAnimation.position.z += PlayerGrab_Fx12(PlayerGrab_Sin(angle) * 24);
                p->facing = ((((angle + 0x20) & 0xFF) >> 6) * 2 + 3) & 7;
                PlayerGrab_SetFacing(p, ((angle + 0x60) & 0xFF) >> 6);
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
                p->tetherFrame++;
                if (p->tetherFrame > 0x59)
                    Player_SetActionPhase(p, 6);
            }
            ParticlePool_EmitTriple((void *)p->unkA88[0], &p->secondaryAnimation.position,
                                    (angle + 0x80) & 0xFF, 1);
            return;
        }
        case 2: {
            struct PlayerGrabTarget *t = (*(struct PlayerGrabTarget **)&gPlayerPendingActor);
            s32 r;
            s32 damage;
            if (t->flags & 1) {
                if (t->flags & 0x10)
                    PlayerGrab_Hit(t, 1, 0x3C);
                else
                    PlayerGrab_Hit(t, 1, 0x3C);
                (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = 0;
                Player_UpdateFacingSide(p);
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
                Player_SetActionPhase(p, 6);
                return;
            }
            if (gUnk_02000510 <= 3 && p->statusValue) {
                if ((p->inputKeys[1] & 2) ||
                    ((p->inputKeys[1] & 1) && p->historyDirectionMode == 1))
                    gUnk_02000530 = 1;
            }
            r = PlayerAnimation_SelectSecondaryKeyframes(p, 0xF0, 0x40);
            if (p->secondaryAnimation.frameIndex == 5 && p->secondaryAnimation.elapsed == 0) {
                s32 amount;
                s32 base;
                s32 hitResult;
                struct SceneExtent vec;
                if (gEngineFlags & 0x1000)
                    amount = 30;
                else
                    amount = Player_ScaleLevelValue(2, 0x60);
                t = (*(struct PlayerGrabTarget **)&gPlayerPendingActor);
                if (t->flags & 0x10)
                    hitResult = PlayerGrab_Hit(t, 0, amount);
                else
                    hitResult = PlayerGrab_Hit(t, 0, amount);
                if (hitResult != 1) {
                    (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = 0;
                    Player_UpdateFacingSide(p);
                    Player_SetActionPhase(p, 6);
                    return;
                }
                Sound_PlaySong(0x39F);
                base = ((p->facing + 5) & 7) << 5;
                vec.x = PlayerGrab_Fx12(PlayerGrab_Sin(base + 0x40) * 64);
                vec.y = 0xE6;
                vec.z = PlayerGrab_Fx12(PlayerGrab_Sin(base) * 64);
                SceneParticle_CreateDefaultPriority(&p->secondaryAnimation.position, &vec, 0, 0, 1);
                {
                    u32 link = gEngineFlags & 0x1000;
                    damage = amount;
                    if (link)
                        damage = 10;
                }
                p->currentStatA += damage;
                if (p->currentStatA >= p->maximumStatA) {
                    damage = p->maximumStatA - p->currentStatA;
                    p->currentStatA = p->maximumStatA;
                }
                if (damage > 0)
                    DamageNumberDisplay_Show(&p->collider, damage, 1,
                                             &p->secondaryAnimation.position,
                                             &p->movementRequestTarget, 0);
                gUnk_02000510++;
                return;
            }
            if (r == 0)
                return;
            if (gUnk_02000530) {
                Player_SetActionPhase(p, 2);
            } else {
                Player_AddControlFlags(p, 8);
                t = (*(struct PlayerGrabTarget **)&gPlayerPendingActor);
                if (t->flags & 0x10)
                    PlayerGrab_Hit(t, 1, 0x3C);
                else
                    PlayerGrab_Hit(t, 1, 0x3C);
                (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = 0;
                Player_SetActionPhase(p, 6);
            }
            gUnk_02000530 = 0;
            return;
        }
        case 3: {
            u16 h = PlayerGrab_GetFrame(p);
            struct PlayerGrabPos vec;
            u8 *headingField;
            if (h == 0) {
                p->targetOrbitAngle = p->unk2D;
                Sound_PlaySong(0x4F6);
                Sound_PlaySong(0x576);
            }
            h = PlayerGrab_GetFrame(p);
            if (h > 5) {
                u32 radius;
                p->targetOrbitAngle += 0x12 + (h >> 1);
                radius = PlayerGrab_GetFrame(p) + 0x4B;
                PlayerGrab_Polar(&vec, p->targetOrbitAngle, radius * 2);
            } else if (h == 5) {
                p->targetOrbitAngle += 0x40;
                PlayerGrab_Polar(&vec, p->targetOrbitAngle, (PlayerGrab_GetFrame(p) >> 1) + 0x14);
            } else {
                PlayerGrab_Polar(&vec, p->targetOrbitAngle, (h >> 1) + 0x28);
            }
            p->secondaryAnimation.position.x += vec.x;
            p->secondaryAnimation.position.z += vec.z;
            p->secondaryAnimation.position.x += p->lockedVelocity.x;
            p->secondaryAnimation.position.z += p->lockedVelocity.z;
            ParticlePool_EmitTriple((void *)p->unkA88[0], &p->secondaryAnimation.position,
                                    (p->targetOrbitAngle + 0xA0) & 0xFF, 1);
            {
                u8 heading = p->targetOrbitAngle;
                headingField = &p->heading;
                *headingField = heading;
            }
            PlayerGrab_SetFacingAt(p, headingField, ((p->targetOrbitAngle + 0x60) >> 6) & 3);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
            if ((p->tetherFrame & 3) == 0)
                TemporaryAnimationPool_CreateEntry(&p->secondaryAnimation, 0x10, 0x200,
                                                   p->menuStatusValues[0], 0);
            PlayerCue_InitAttackCollider(p, &p->secondaryAnimation.position, 2, 0x20);
            p->tetherFrame++;
            if (p->tetherFrame > 0x23)
                Player_SetActionPhase(p, 4);
            return;
        }
        case 4:
            ActorSequence_Begin(p, 0, 0);
            return;
        case 5:
            p->actionEighteenParameter = 0x16F;
            p->actionEighteenDuration = 0x20;
            Player_UpdateFacingSide(p);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            p->tetherFrame++;
            if (p->tetherFrame > 0x1F)
                ActorSequence_Begin(p, 0, 0);
            PlayerGrab_CopyPosition(p);
            Player_AddStatusFlags(p, 0x4000);
            return;
        case 6: {
            s32 angle;
            Player_AddControlFlags(p, 8);
            if (p->tetherFrame == 0) {
                s32 dx = p->movementStart.x - p->secondaryAnimation.position.x;
                s32 dz = p->movementStart.z - p->secondaryAnimation.position.z;
                s32 speed = Div((u32)((u16)Sqrt(dx * dx + dz * dz)) << 4, 12);
                speed = PlayerGrab_Fx4(speed);
                p->cueSpeed = speed;
                p->actionEighteenParameter = 0x16F;
                p->actionEighteenDuration = 0x20;
            }
            Player_UpdateFacingSide(p);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            angle = Math_Atan2(p->movementStart.x - p->secondaryAnimation.position.x,
                               p->movementStart.z - p->secondaryAnimation.position.z);
            p->secondaryAnimation.position.x +=
                PlayerGrab_Fx12(PlayerGrab_Sin(angle + 0x40) * p->cueSpeed);
            p->secondaryAnimation.position.z +=
                PlayerGrab_Fx12(PlayerGrab_Sin(angle) * p->cueSpeed);
            p->terrainRecoveryCounter =
                PlayerGrab_Fx12(PlayerGrab_Sin((p->tetherFrame + 4) << 3) << 6);
            p->tetherFrame++;
            if (p->tetherFrame > 0xB) {
                p->terrainRecoveryCounter = 0;
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
