#include "global.h"
struct FrameDisplayPosition {
    u32 words[2];
};
struct FrameDisplayAnimation {
    u8 unknown0[0x20];
    struct FrameDisplayPosition position;
};
struct FrameDisplaySprite {
    u32 flags;
    u8 unknown4[3];
    u8 dirty;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xa];
    struct FrameDisplayPosition position;
};
struct FrameDisplay {
    u8 unknown0[0x34];
    struct FrameDisplaySprite sprite;
    u8 unknown58[0xc];
    s32 compactFrame;
    u8 unknown68[8];
    u8 *flip;
    struct FrameDisplayAnimation *animation;
};
static inline void SetFrame(struct FrameDisplaySprite *sprite, s32 frame)
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
    p->sprite.dirty = 1;
}
