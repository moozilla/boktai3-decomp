#include "engine/shared/global.h"
#include "player/player.h"
#include "engine/shared/game_state_flags.h"
#include "player/player_slot_status.h"

void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);
s32 RepeatedEffect_RequestNearby(void *);
s32 Player_IsNormalActionAllowed(u32);
void Sound_PlaySong(u32);
struct PlayerRef {
    struct Player *player;
};
static inline struct PlayerRef PlayerRef_FromPlayer(struct Player *p)
{
    struct PlayerRef ref;
    ref.player = p;
    return ref;
}
static inline u32 PlayerRef_TestMovementFlag(struct PlayerRef ref)
{
    struct Player *p = ref.player;
    if (!p)
        return 0;
    return Player_ReadStatusBits(&p->actionFlags, 0x20000);
}
static inline u8 IsMode(struct Player *p, u32 mode)
{
    u16 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == mode)
        result = 1;
    return result;
}
void Player_EmitMovementEffects(struct Player *p, s32 quiet)
{
    if ((p->animation.frameIndex & 3) || p->animation.elapsed)
        return;
    if (p && ((p->active && p->sceneMode == 0) || Player_ReadStatusBits(&p->actionFlags, 4)))
        return;
    if (PlayerRef_TestMovementFlag(PlayerRef_FromPlayer(p))) {
        if (p->animation.frameIndex == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5ae);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x5ad);
        RepeatedEffect_RequestNearby(&p->position);
        return;
    }
    if (RepeatedEffect_RequestNearby(&p->position)) {
        if (p->animation.frameIndex == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x163);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x162);
        return;
    }
    if (Player_IsNormalActionAllowed(p->movementStatus) && !(p && IsMode(p, 5))) {
        if (p->animation.frameIndex == 0)
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x194);
        else
            Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x193);
        return;
    }
    if (!quiet && !PlayerRef_TestMovementFlag(PlayerRef_FromPlayer(p))) {
        if (p && IsMode(p, 5)) {
            if (p->animation.frameIndex == 0)
                Sound_PlaySong(0xc8);
            else
                Sound_PlaySong(0xc7);
        } else if (p->tetherStatusFlags || !p->velocityScaleAxis) {
            if (p->animation.frameIndex == 0)
                Sound_PlaySong(0xca);
            else
                Sound_PlaySong(0xc9);
        } else {
            if (p->animation.frameIndex == 0)
                Sound_PlaySong(0x28e);
            else
                Sound_PlaySong(0x28d);
        }
    }
    if (GameState_TestWorldFlags(1))
        Player_PlayPositionEffect(p, &p->position, 1, 2, 0, 0, 0);
}
