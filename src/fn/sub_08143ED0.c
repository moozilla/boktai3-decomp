#include "global.h"
#include "player.h"
#include "game_state.h"

// confidence=low: updates movement and step cues while constraining the tether target.
struct PlayerMovementCueFields {
    u8 unknown0[0x292];
    u16 normalAction;
};
struct PlayerLockedFacingFields {
    u8 unknown0[0x48F];
    u8 lockedFacing;
};
extern const s16 gUnk_086149C4[];
s32 Player_ConsumeStateEntry(void *);
void ActorSequence_Begin(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_CalculateMovementSpeed(void *);
u32 Player_GetSmoothedDirection(void *);
void Player_UpdateFacingSide(void *);
u32 RepeatedEffect_RequestNearby(void *);
u32 Player_IsNormalActionAllowed(u32);
void Player_PlayPositionEffect(void *, void *, s32, u32, void *, u32, u32);
void Sound_PlaySong(u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_ConstrainTetherTarget(void *, s32, s32, s32);
static inline s32 scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
static inline u8 modeFive(struct Player *p)
{
    u16 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
static inline u32 flags(u32 *p, u32 mask)
{
    return *p & mask;
}
struct PlayerRef {
    struct Player *player;
};
static inline struct PlayerRef PlayerRef_FromPlayer(struct Player *p)
{
    struct PlayerRef ref;
    ref.player = p;
    return ref;
}
static inline u32 FlagMovement(struct PlayerRef ref)
{
    struct Player *p = ref.player;
    if (!p)
        return 0;
    return flags(&p->actionFlags, 0x20000);
}
static inline u32 GlobalMask(u32 mask)
{
    return gGameState->flags868 & mask;
}
static inline u8 HasGlobalFlag(void)
{
    if (GlobalMask(1))
        return 1;
    return 0;
}
void Player_UpdateTetherMovement(struct Player *p)
{
    s32 animation, rate, moving, cosine, sine, angle, speed;
    if (Player_ConsumeStateEntry(p))
        ActorSequence_Begin(p, 6, 0);
    p->statusFlags &= ~1;
    if ((s16)p->directionHistory[0] < 0 || p->cameraOffsetActive != 0) {
        animation = ActorVariant_GetTableValue(p->actionMode, 16);
        if (p->movementVelocityLocked)
            angle = ((((struct PlayerLockedFacingFields *)p)->lockedFacing + 1) & 7) << 5;
        else
            angle = ((p->facing + 1) & 7) << 5;
        cosine = gUnk_086149C4[(angle + 64) & 255];
        sine = gUnk_086149C4[angle & 255];
        moving = 0;
        rate = 64;
    } else {
        animation = ActorVariant_GetTableValue(p->actionMode, 17);
        speed = Player_CalculateMovementSpeed(p);
        moving = 1;
        p->facing = Player_GetSmoothedDirection(p);
        Player_UpdateFacingSide(p);
        if (p->movementVelocityLocked) {
            rate = 32;
            angle = ((((struct PlayerLockedFacingFields *)p)->lockedFacing + 1) & 7) << 5;
            cosine = gUnk_086149C4[(angle + 64) & 255];
            sine = gUnk_086149C4[angle & 255];
        } else {
            if (speed == 8)
                rate = 64;
            else if (speed < 8)
                rate = 64 + ((8 - speed) << 3);
            else
                rate = 64 - ((speed - 8) << 2);
            angle = ((p->facing + 1) & 7) << 5;
            cosine = gUnk_086149C4[(angle + 64) & 255];
            sine = gUnk_086149C4[angle & 255];
            if (p->actionCueFlags & 3) {
                p->velocityX = -scale12(cosine * speed);
                p->velocityZ = -scale12(sine * speed);
                if (p->velocityScaleAxis == 1) {
                    if (p->velocityZ < 0)
                        p->velocityZ = scale6(p->velocityZ * 40);
                    else
                        p->velocityZ = scale6(p->velocityZ * 52);
                } else if (p->velocityScaleAxis == 2) {
                    if (p->velocityX < 0)
                        p->velocityX = scale6(p->velocityX * 40);
                    else
                        p->velocityX = scale6(p->velocityX * 52);
                }
            }
        }
    }
    if (moving && (s16)p->directionHistory[1] >= 0 && !(p->actionCueFlags & 3) &&
        !p->actionCueCounter) {
        if (p && ((p->active && p->sceneMode == 0) || flags(&p->actionFlags, 4)))
            goto animate;
        if (FlagMovement(PlayerRef_FromPlayer(p))) {
            if (!p->actionCueFlags)
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5AE);
            else
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5AD);
        } else if (RepeatedEffect_RequestNearby(&p->position)) {
            if (!p->actionCueFlags)
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x163);
            else
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x162);
        } else if (Player_IsNormalActionAllowed(
                       ((struct PlayerMovementCueFields *)p)->normalAction) &&
                   !(p && modeFive(p))) {
            if (!p->actionCueFlags)
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x194);
            else
                Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x193);
        } else {
            if (p->tetherStatusFlags || !p->velocityScaleAxis) {
                if (!p->actionCueFlags)
                    Sound_PlaySong(0xCA);
                else
                    Sound_PlaySong(0xC9);
            } else {
                if (!p->actionCueFlags)
                    Sound_PlaySong(0x28E);
                else
                    Sound_PlaySong(0x28D);
            }
            if (HasGlobalFlag())
                Player_PlayPositionEffect(p, &p->position, 1, 2, 0, 0, 0);
        }
    }
animate:
    Player_SetAnimation(p, animation, rate, 1);
    if (moving || p->movementVelocityLocked)
        Player_ConstrainTetherTarget(p, cosine, sine, 150);
}
