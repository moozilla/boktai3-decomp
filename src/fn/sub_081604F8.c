#include "global.h"
#include "frame_display.h"
void AnimData_Load(void *, s32);
void Sprite_Init(void *, void *, s32);
s32 ActorDebris_Init(struct FrameDisplay *display, struct SceneInputWidget *animation, u8 *heading,
                     u8 *flip)
{
    u8 *data = display->animationData;
    struct ContactSwarmSprite *sprite = &display->sprite;
    AnimData_Load(data, 0xB250);
    Sprite_Init(sprite, data, 1);
    sprite->frame = 0;
    sprite->position = animation->position;
    display->frameCounter = 0;
    display->compactFrame = 0;
    display->animation = animation;
    display->heading = heading;
    display->flip = flip;
    return 0;
}
