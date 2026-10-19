#include "global.h"
/* Vanargand_UpdateDescendTrack: starts the unmirrored animation, then runs two motion-track legs (origin heights 0x300 then 0x100) and on completion resets the part animation and flags the state finished (provisional name) */
#define Vanargand_UpdateDescendTrack sub_081AA438
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation;
    u8 padB2[0x18C - 0xB2];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x5C2 - 0x55E];
    u16 counter;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

static inline void initialize(void *track, void *data, s16 x, s16 y, s16 z, s16 *target, u32 mode,
                              s16 *origin)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}

void sub_081AA438(struct Vanargand *p)
{
    void *animation = p->animation;
    u32 mask = 4;
    s16 first[4], second[4];
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        p->phase = 0;
        p->timer = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 2, 1, mask);
        Anim_SetFrameDurationScale(animation, 0x20);
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                initialize(p->track, p->motionData, 0x400, 0x300, 0x400, 0, mask, first);
                p->timer++;
            }
            MotionTrack_Update(p->track);
            if (complete(p)) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                initialize(p->track, p->motionData, 0x400, 0x100, 0x400, 0, 2, second);
                p->timer++;
            }
            MotionTrack_Update(p->track);
            if (complete(p)) {
                {
                    struct VanargandPart *part = &p->part;
                    part->state = 7;
                }
                p->partActive = 0;
                p->counter = 1;
                p->finished = 1;
                p->phase = 2;
                p->timer = 0;
            }
            break;
        case 2:
        default:
            break;
    }
}
