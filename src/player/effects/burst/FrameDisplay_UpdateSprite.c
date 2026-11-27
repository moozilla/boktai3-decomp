#include "player/effects/frame_display.h"

static inline void SetFrame(struct ContactSwarmSprite *sprite, s32 frame)
{
    sprite->frame = frame;
}

s32 FrameDisplay_GetCompactFrameIndex(struct FrameDisplay *);
void FrameDisplay_UpdateSprite(struct FrameDisplay *p)
{
    s32 frame = FrameDisplay_GetCompactFrameIndex(p);
    if (frame != p->compactFrame || (p->sprite.flags & 4) != *p->flip) {
        p->compactFrame = frame;
        if (frame == -1) {
            p->sprite.flags |= 1;
        } else {
            p->sprite.flags &= ~1;
            SetFrame(&p->sprite, frame);
        }
        if (*p->flip)
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    p->sprite.position = p->animation->position;
    p->sprite.priority = 1;
}
