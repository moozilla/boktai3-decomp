#include "global.h"

struct MovementEffectPosition {
    s16 x, y, z;
    u16 unknown6;
};
struct MovementEffectPlayer {
    u8 unknown0[0x30];
    struct MovementEffectPosition position;
    u8 unknown38[0x60];
    u16 actionCueCounter;
    u8 unknown9A[6];
    u16 actionCueFlags;
    u8 unknownA2[0x1ED];
    u8 velocityScaleAxis;
    u8 unknown290[2];
    u16 normalAction; /* action selector, tentative */
    u8 unknown294[0x1B4];
    u32 actionFlags;
    u8 unknown44C[0x28];
    u16 tetherStatusFlags;
    u8 unknown476[0xBE];
    u16 sceneMode;
    u16 unknown536;
    u16 active;
};
struct MovementEffectState {
    u8 unknown0[0x868];
    u32 flags;
};
extern struct MovementEffectState *gGameState;
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);
s32 RepeatedEffect_RequestNearby(void *);
s32 Player_IsNormalActionAllowed(u32);
void Sound_PlaySong(u32);
static inline u32 TestFlagBits(u32 *v, u32 m)
{
    return *v & m;
}
struct PlayerRef {
    struct MovementEffectPlayer *player;
};
static inline struct PlayerRef PlayerRef_FromPlayer(struct MovementEffectPlayer *p)
{
    struct PlayerRef ref;
    ref.player = p;
    return ref;
}
static inline u32 PlayerRef_TestMovementFlag(struct PlayerRef ref)
{
    struct MovementEffectPlayer *p = ref.player;
    if (!p)
        return 0;
    return TestFlagBits(&p->actionFlags, 0x20000);
}
static inline u8 IsMode(struct MovementEffectPlayer *p, u32 mode)
{
    u16 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == mode)
        result = 1;
    return result;
}
static inline u32 GlobalMask(u32 mask)
{
    return gGameState->flags & mask;
}
static inline u8 HasGlobalFlag(void)
{
    if (GlobalMask(1))
        return 1;
    return 0;
}
void Player_EmitMovementEffects(struct MovementEffectPlayer *p, s32 quiet)
{
    if ((p->actionCueFlags & 3) || p->actionCueCounter)
        return;
    if (p && ((p->active && p->sceneMode == 0) || TestFlagBits(&p->actionFlags, 4)))
        return;
    if (PlayerRef_TestMovementFlag(PlayerRef_FromPlayer(p))) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5ae);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5ad);
        RepeatedEffect_RequestNearby(&p->position);
        return;
    }
    if (RepeatedEffect_RequestNearby(&p->position)) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x163);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x162);
        return;
    }
    if (Player_IsNormalActionAllowed(p->normalAction) && !(p && IsMode(p, 5))) {
        if (p->actionCueFlags == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x194);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x193);
        return;
    }
    if (!quiet && !PlayerRef_TestMovementFlag(PlayerRef_FromPlayer(p))) {
        if (p && IsMode(p, 5)) {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0xc8);
            else
                Sound_PlaySong(0xc7);
        } else if (p->tetherStatusFlags || !p->velocityScaleAxis) {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0xca);
            else
                Sound_PlaySong(0xc9);
        } else {
            if (p->actionCueFlags == 0)
                Sound_PlaySong(0x28e);
            else
                Sound_PlaySong(0x28d);
        }
    }
    if (HasGlobalFlag())
        Player_PlayPositionEffect(p, &p->position, 1, 2, 0, 0, 0);
}
