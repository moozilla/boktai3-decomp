#include "global.h"

/* tentative: plays the action cue sound or positional effect chosen by mode and tether */

#include "player.h"
#include "player_slot_status.h"
u32 Player_IsFlaggedWithoutGlobalModifiers(u32);
void Player_PlayLocalSong(s32, u32);
void Player_EmitPositionEffect(struct Player *s, const struct SceneVec *position, s32 height,
                               u32 variant, const void *effect, u32 flags, u32 song);
static inline u8 IsModeFive(struct Player *p)
{
    u32 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
void Player_PlayActionCueSound(struct Player *p, s32 quiet)
{
    if (p->animation.frameIndex & 3)
        return;
    if (p->animation.elapsed)
        return;
    if (p && ((p->active && p->sceneMode == 0) || Player_ReadStatusBits(&p->actionFlags, 4)))
        return;
    if (Player_IsFlaggedWithoutGlobalModifiers(p->movementStatus) && !(p && IsModeFive(p))) {
        if (p->animation.frameIndex == 0)
            Player_EmitPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x194);
        else
            Player_EmitPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x193);
        return;
    }
    if (quiet)
        return;
    if (p && IsModeFive(p)) {
        if (p->animation.frameIndex == 0)
            Player_PlayLocalSong(p->slot, 0xC8);
        else
            Player_PlayLocalSong(p->slot, 0xC7);
    } else if (p->tetherStatusFlags || !p->velocityScaleAxis) {
        if (p->animation.frameIndex == 0)
            Player_PlayLocalSong(p->slot, 0xCA);
        else
            Player_PlayLocalSong(p->slot, 0xC9);
    } else {
        if (p->animation.frameIndex == 0)
            Player_PlayLocalSong(p->slot, 0x28E);
        else
            Player_PlayLocalSong(p->slot, 0x28D);
    }
}
