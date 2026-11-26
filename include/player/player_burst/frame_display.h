#ifndef GUARD_FRAME_DISPLAY_H
#define GUARD_FRAME_DISPLAY_H

#include "engine/shared/task.h"
#include "actors/task_4_e01/contact_swarm_sprite.h"
#include "engine/shared/scene_input_widget.h"

struct FrameDisplay {
    struct Task task;
    u8 animationData[0x1C];
    struct ContactSwarmSprite sprite;
    u32 flags;
    s32 compactFrame;
    u32 frameCounter;
    u8 *heading;
    u8 *flip;
    struct SceneInputWidget *animation; /* parent body animation the display follows */
};

typedef char FrameDisplaySizeCheck[sizeof(struct FrameDisplay) == 0x78 ? 1 : -1];

void FrameDisplay_UpdateSprite(struct FrameDisplay *display);

#endif
