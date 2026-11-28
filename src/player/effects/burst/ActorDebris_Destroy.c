#include "player/effects/frame_display.h"

s32 Sprite_Remove(struct ContactSwarmSprite *);

s32 ActorDebris_Destroy(struct FrameDisplay *display)
{
    return Sprite_Remove(&display->sprite);
}
