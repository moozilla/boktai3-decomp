#include "global.h"
#include "ram.h"
#include "engine_globals.h"
#include "player.h"
#include "scene_object.h"

struct PlayerGrabHalf {
    u16 value : 16;
};

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
struct PlayerGrabTarget *sub_0814C248(struct Player *);
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

struct PlayerGrabSecondary {
    u8 unknown0[0xEC];
    struct PlayerAnimation animation;
    u16 elapsed;
    u8 unknownE[6];
    u16 frame;
    u8 unknown16[10];
    struct SceneVec position;
    u8 unknown28[18];
    u16 spriteResource;
    u8 unknown3C[12];
    u32 variantResource;
};
struct PlayerGrabGrabContext {
    u8 unknown0[0xA88];
    void *particlePool;
    struct SceneVec origin, destination;
    u16 duration, returnSpeed;
    u8 heading;
};
#define PlayerGrab_Secondary(p) ((struct PlayerGrabSecondary *)(p))
#define PlayerGrab_Context(p)   ((struct PlayerGrabGrabContext *)(p))

struct PlayerGrabCoordinates {
    u8 unknown0[0x10C];
    s16 x, y, z;
};
struct PlayerGrabStoredCoordinates {
    u8 unknown0[0xA8C];
    s16 x, y, z;
};
struct PlayerGrabPlanarVelocity {
    u8 unknown0[0x490];
    u16 x;
    u8 unknown492[2];
    u16 z;
};
#define PlayerGrab_Coordinates(p)       ((struct PlayerGrabCoordinates *)(p))
#define PlayerGrab_StoredCoordinates(p) ((struct PlayerGrabStoredCoordinates *)(p))
#define PlayerGrab_PlanarVelocity(p)    ((struct PlayerGrabPlanarVelocity *)(p))
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
    PlayerGrab_Secondary(p)->position = p->position;
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
static inline void PlayerGrab_ClearAnimation(struct PlayerAnimation *animation)
{
    animation->flags &= ~1;
}

static inline void PlayerGrab_SetFlags(struct Player *p, u32 flags)
{
    p->statusFlags |= flags;
}
static inline void PlayerGrab_SetModeFlags(struct Player *p, u32 mask)
{
    p->statusFlags = (p->statusFlags & mask) | 0x60000;
}
static inline u32 PlayerGrab_Mask(u32 value, u32 mask)
{
    mask &= value;
    return mask;
}

// confidence=low: Player grab/throw action update; entry setup, then an 8-phase switch (reach, pull, throw, release, ...).
#define Player_UpdateGrabMotion sub_0814C3A4
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
        PlayerGrab_ClearAnimation(&p->secondaryAnimation);
        PlayerGrab_Secondary(p)->position = p->previousPosition;
        {
            u16 resource = p->spriteResource;
            PlayerGrab_Secondary(p)->spriteResource = resource;
        }
        PlayerGrab_Secondary(p)->variantResource = p->actionVariantResource;
        p->movementStart = p->position;
        Player_SetPendingCallback(p, Player_OnGrabMotionExit);
        if (PlayerGrab_Context(p)->particlePool == 0)
            PlayerGrab_Context(p)->particlePool = ParticlePool_Create();
        if (gEngineFlags & 0x1000)
            WirelessGame_ResetCyclingItemLatch();
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }
    Player_AddControlFlags(p, 9);
    PlayerGrab_SetModeFlags(p, ~0x4000);
    switch (p->tetherPhase) {
        case 0: {
            u16 v = p->tetherFrame;
            struct PlayerGrabHalf mask;
            s32 low;
            mask.value = 7;
            low = PlayerGrab_Mask(v, mask.value);
            if (low < mask.value - (v >> 3))
                PlayerAnimation_SelectSecondaryKeyframes(
                    p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
            else
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            PlayerGrab_CopyPosition(p);
            (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = sub_0814C248(p);
            if ((*(struct PlayerGrabTarget **)&gPlayerPendingActor) == 0) {
                PlayerGrab_SetFlags(p, 0x4000);
            } else {
                Player_SetActionPhase(p, 1);
                Sound_PlaySong(0x53C);
            }
            p->tetherFrame++;
            if (p->tetherFrame <= 0x1D)
                return;
            (*(struct PlayerGrabTarget **)&gPlayerPendingActor) = sub_0814C248(p);
            if ((*(struct PlayerGrabTarget **)&gPlayerPendingActor))
                return;
            Player_SetActionPhase(p, 5);
            return;
        }
        case 1: {
            struct PlayerGrabTarget *t;
            s32 dx = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->x -
                     PlayerGrab_Coordinates(p)->x;
            s32 dy;
            s32 dz;
            s32 angle;
            dy = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->y -
                 PlayerGrab_Coordinates(p)->y;
            dz = PlayerGrab_TargetPosition((struct PlayerGrabTarget *)gPlayerPendingActor)->z -
                 PlayerGrab_Coordinates(p)->z;
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
                        s32 base;
                        s32 octantMask;
                        s32 angleMask;
                        s32 quarter;
                        {
                            s32 adjusted = angle + 0x20;
                            s32 facing;
                            angleMask = 255;
                            adjusted &= angleMask;
                            facing = (adjusted >> 6) * 2 + 3;
                            octantMask = 7;
                            facing &= octantMask;
                            p->facing = facing;
                        }
                        if (!((*(struct PlayerGrabTarget **)&gPlayerPendingActor)->flags & 0x10))
                            ((u8 *)(*(struct PlayerGrabTarget **)&gPlayerPendingActor)->body)[9] =
                                ((p->facing + 5) & octantMask) << 5;
                        base = ((angle + 0x60) & angleMask) >> 6;
                        quarter = base;
                        PlayerGrab_SetFacing(p, quarter);
                        PlayerAnimation_SelectSecondaryKeyframes(p, 0xF0, 0x40);
                        Player_SetActionPhase(p, 2);
                    }
                }
            } else {
                s32 quarter;
                if (dy > 0x18)
                    PlayerGrab_Coordinates(p)->y += 0x18;
                else if (dy < -0x18)
                    PlayerGrab_Coordinates(p)->y -= 0x18;
                PlayerGrab_Coordinates(p)->x += PlayerGrab_Fx12(PlayerGrab_Sin(angle + 0x40) * 24);
                PlayerGrab_Coordinates(p)->z += PlayerGrab_Fx12(PlayerGrab_Sin(angle) * 24);
                {
                    s32 facing = ((((angle + 0x20) & 0xFF) >> 6) * 2 + 3) & 7;
                    p->facing = facing;
                }
                quarter = (((angle + 0x60) & 0xFF) >> 6);
                PlayerGrab_SetFacing(p, quarter);
                PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
                p->tetherFrame++;
                if (p->tetherFrame > 0x59)
                    Player_SetActionPhase(p, 6);
            }
            ParticlePool_EmitTriple((void *)PlayerGrab_Context(p)->particlePool,
                                    &PlayerGrab_Secondary(p)->position, (angle + 0x80) & 0xFF, 1);
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
            if (PlayerGrab_Secondary(p)->frame == 5 && PlayerGrab_Secondary(p)->elapsed == 0) {
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
                SceneParticle_CreateDefaultPriority(&PlayerGrab_Secondary(p)->position, &vec, 0, 0,
                                                    1);
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
                                             &PlayerGrab_Secondary(p)->position,
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
                u8 heading = p->unk2D;
                PlayerGrab_Context(p)->heading = heading;
                Sound_PlaySong(0x4F6);
                Sound_PlaySong(0x576);
            }
            h = PlayerGrab_GetFrame(p);
            if (h > 5) {
                u32 radius;
                PlayerGrab_Context(p)->heading += 0x12 + (h >> 1);
                radius = PlayerGrab_GetFrame(p) + 0x4B;
                PlayerGrab_Polar(&vec, PlayerGrab_Context(p)->heading, radius * 2);
            } else if (h == 5) {
                PlayerGrab_Context(p)->heading += 0x40;
                PlayerGrab_Polar(&vec, PlayerGrab_Context(p)->heading,
                                 (PlayerGrab_GetFrame(p) >> 1) + 0x14);
            } else {
                PlayerGrab_Polar(&vec, PlayerGrab_Context(p)->heading, (h >> 1) + 0x28);
            }
            PlayerGrab_Coordinates(p)->x += vec.x;
            PlayerGrab_Coordinates(p)->z += vec.z;
            PlayerGrab_Coordinates(p)->x += PlayerGrab_PlanarVelocity(p)->x;
            PlayerGrab_Coordinates(p)->z += PlayerGrab_PlanarVelocity(p)->z;
            ParticlePool_EmitTriple((void *)PlayerGrab_Context(p)->particlePool,
                                    &PlayerGrab_Secondary(p)->position,
                                    (PlayerGrab_Context(p)->heading + 0xA0) & 0xFF, 1);
            {
                u8 heading = PlayerGrab_Context(p)->heading;
                headingField = &p->heading;
                *headingField = heading;
            }
            PlayerGrab_SetFacingAt(p, headingField,
                                   ((PlayerGrab_Context(p)->heading + 0x60) >> 6) & 3);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xEE, 0x40);
            if ((p->tetherFrame & 3) == 0)
                TemporaryAnimationPool_CreateEntry(&p->secondaryAnimation, 0x10, 0x200,
                                                   p->menuStatusValues[0], 0);
            PlayerCue_InitAttackCollider(p, &PlayerGrab_Secondary(p)->position, 2, 0x20);
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
            PlayerGrab_SetFlags(p, 0x4000);
            return;
        case 6: {
            s32 angle;
            Player_AddControlFlags(p, 8);
            if (p->tetherFrame == 0) {
                s32 dx = PlayerGrab_StoredCoordinates(p)->x - PlayerGrab_Coordinates(p)->x;
                s32 dz = PlayerGrab_StoredCoordinates(p)->z - PlayerGrab_Coordinates(p)->z;
                s32 speed = Div((u32)((u16)Sqrt(dx * dx + dz * dz)) << 4, 12);
                speed = PlayerGrab_Fx4(speed);
                PlayerGrab_Context(p)->returnSpeed = speed;
                p->actionEighteenParameter = 0x16F;
                p->actionEighteenDuration = 0x20;
            }
            Player_UpdateFacingSide(p);
            PlayerAnimation_SelectSecondaryKeyframes(p, 0xE9, 0x40);
            angle = Math_Atan2(PlayerGrab_StoredCoordinates(p)->x - PlayerGrab_Coordinates(p)->x,
                               PlayerGrab_StoredCoordinates(p)->z - PlayerGrab_Coordinates(p)->z);
            PlayerGrab_Coordinates(p)->x +=
                PlayerGrab_Fx12(PlayerGrab_Sin(angle + 0x40) * PlayerGrab_Context(p)->returnSpeed);
            PlayerGrab_Coordinates(p)->z +=
                PlayerGrab_Fx12(PlayerGrab_Sin(angle) * PlayerGrab_Context(p)->returnSpeed);
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
