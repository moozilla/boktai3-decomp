#include "global.h"

// confidence=low: starts a target-directed motion track and emits afterimages.
#define Vanargand_UpdateTargetDash sub_081A9178
struct Vector {
    s16 x, y, z, pad;
};
struct DashActor {
    u8 pad0[0x54];
    s16 position[4];
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[2];
    u8 phase, frame;
    u32 timer;
    u8 padA8[7];
    u8 finished, pending, pendingAnimation;
    u8 padB2[0x104 - 0xB2];
    void *callback;
    u8 pad108[8];
    s16 *target;
    u8 pad114[0x18C - 0x114];
    u8 animation[0x60];
    u8 drawable[0x534 - 0x1EC];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0xF44 - 0x55E];
    void *afterimages;
};
extern const s16 gUnk_086149C4[];
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
void VanargandAfterimagePool_Spawn(void *, void *);
s32 Math_Atan2(s32, s32);
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline void SetVector(struct Vector *v, s32 x, s32 y, s32 z)
{
    s16 *component = &v->x;
    *component = x;
    v->y = y;
    v->z = z;
}
static inline void AddVectorInPlace(struct Vector *v, struct Vector *a)
{
    u16 av;
    u32 bv;
    v->x += a->x;
    av = v->y;
    bv = (u16)a->y;
    v->y = av + bv;
    v->z += a->z;
}

static inline u8 IsComplete(struct DashActor *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}
static inline void SetPolar(struct Vector *v, s32 angle, s32 radius)
{
    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

void Vanargand_UpdateTargetDash(struct DashActor *p)
{
    struct DashActor *self = p;
    u32 mask = 0x48;
    u8 result;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;

    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        p->phase = result;
        p->timer = result;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, p->animation, p->drawable, 4, 1, 0);
                Sound_PlaySong(0x586);
                p->timer++;
            }
            if (p->frame > 10) {
                p->phase = 1;
                p->timer = 0;
            }
            break;

        case 1:
            if (p->timer == 0) {
                void *track = p->track;
                s16 *target = p->target;
                struct Vector center;
                struct Vector destination;
                s32 dx;
                s32 dz;
                s16 *first;
                SetVector(&center, 640, 768, 640);
                first = &center.x;

                dx = first[0] - target[0];
                dz = 640 - target[2];
                if (((dx * dx) + (dz * dz)) <= 0x100000) {
                    MotionTrack_Initialize(track, p->position, target, 0, 3);
                } else {
                    SetPolar(&destination, Math_Atan2(target[0] - first[0], target[2] - 640), 1024);
                    AddVectorInPlace(&destination, &center);
                    destination.y = target[1];
                    MotionTrack_Initialize(track, p->position, &destination, 0, 3);
                }
                p->timer++;
            }
            MotionTrack_Update(self->track);
            if (IsComplete(self)) {
                u8 next = 2;
                u32 zero = 0;
                p->phase = next;
                p->timer = zero;
            } else {
                VanargandAfterimagePool_Spawn(self->afterimages, p->position);
            }
            break;

        case 2:
            p->finished = 1;
            break;
    }
}
