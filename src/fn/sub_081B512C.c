#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"

struct ActorStatusPart {
    u32 spriteFlags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
    struct SceneVec position;
    u8 unk_24[0x24];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *anchor;
    u8 unk_94[8];
    struct SceneVec offset;
    u8 unk_A4[0xb8];
    u32 variant, flags, state, timer;
    u8 action, previousAction, reset, enter, finished;
    u8 unk_171[0x2b];
    u32 countdown;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);

static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

static inline void AddOffset(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = (u16)b->z + (u16)a->z;
}

static inline void CopyRaised(struct SceneVec *out, struct SceneVec *origin)
{
    s32 y;
    out->x = origin->x;
    y = (u16)origin->y;
    y += 256;
    out->y = y;
    out->z = origin->z;
}

static inline void SetState(struct ActorStatusPart *p, u32 state)
{
    u32 next = state;
    p->state = next;
    p->timer = 0;
}

static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}

// Tentative role: oscillate a Vanargand part vertically with a pause at each end.
void Vanargand_UpdatePartVerticalOscillation(u8 *unused, struct ActorStatusPart *p)
{
    struct SceneVec lowerTarget, upperTarget;
    u8 *enter;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            u32 entered;
            void *motion;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            motion = p->motion;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                SetVector(&lowerTarget, zero, -256, zero);
                AddOffset(&lowerTarget, &p->offset, &lowerTarget);
                MotionTrack_Initialize(motion, &p->offset, &lowerTarget, 0, 4);
            }
            MotionTrack_Update(motion);
            if (MotionEnded(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }
        case 1:
            if (++p->timer > 30) {
                SetState(p, 2);
                p->enter = 1;
            }
            break;
        case 2: {
            u32 entered;
            void *motion;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            motion = p->motion;
            if (entered) {
                *enter = 0;
                CopyRaised(&upperTarget, &p->offset);
                MotionTrack_Initialize(motion, &p->offset, &upperTarget, 0, 4);
            }
            MotionTrack_Update(motion);
            if (MotionEnded(p)) {
                SetState(p, 3);
                *enter = 1;
            }
            break;
        }
        case 3:
            if (++p->timer > 30) {
                SetState(p, 0);
                p->enter = 1;
            }
            break;
    }
}
