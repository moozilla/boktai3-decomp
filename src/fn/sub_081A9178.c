// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: vector alias lifetimes, timer/track spills and register allocation.
// confidence=low: starts a target-directed motion track and emits afterimages.
#define Vanargand_UpdateTargetDash sub_081A9178
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
static inline void SetVector(s16 *v, s32 x, s32 y, s32 z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static inline void AddVector(s16 *v, s16 *a, s16 *b)
{
    u16 av = a[0];
    s8 bv = b[0];
    v[0] = av + bv;
    av = b[1];
    bv = a[1];
    v[1] = av + bv;
    av = a[2];
    bv = b[2];
    v[2] = av + bv;
}

static inline u8 IsComplete(struct DashActor *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}
inline static void SetVector_081A9178(s16 *v, s32 x, s32 y, s32 z)
{
    {
        typeof(v[0]) *q1 = &v[0];
        *q1 = x;
    }
    v[1] = y;
    v[2] = z;
}

inline static void AddVector_081A9178(s16 *v, s16 *a, s16 *b)
{
    u16 av = a[0];
    s8 bv = b[0];
    v[0] = av + bv;
    av = b[1];
    bv = a[1];
    v[1] = av + bv;
    av = a[2];
    {
        typeof(v[2]) *q1 = &v[2];
        *q1 = av + bv;
    }
    bv = b[2];
}

void Vanargand_UpdateTargetDash(struct DashActor *p)
{
    register struct DashActor *self asm("r10") = p;
    u32 mask = 0x48;
    u8 result;
    register u8 *phase asm("r9");
    asm("" : "+r"(self));
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    phase = &p->phase;
    asm("" : "+r"(&p->phase));
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        *(&p->phase) = result;
        p->timer = result;
    }
    switch (*(&p->phase)) {
        case 0:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, p->animation, p->drawable, 4, 1, 0);
                Sound_PlaySong(0x586);
                p->timer++;
            }
            if (p->frame > 10) {
                *(&p->phase) = 1;
                p->timer = 0;
            }
            break;

        case 1:
            if (p->timer == 0) {
                void *track = p->track;
                s16 *target = p->target;
                s16 center[4];
                s16 destination[4];
                s32 dx;
                s32 dz;
                s16 *first;
                SetVector_081A9178(center, 640, 768, 640);
                first = center;
                asm("" : "+r"(first));
                dx = first[0] - target[0];
                dz = 640 - target[2];
                if (((dx * dx) + (dz * dz)) <= 0x100000) {
                    MotionTrack_Initialize(track, p->position, target, 0, 3);
                } else {
                    s32 radius = 1024;
                    s32 angle = Math_Atan2(target[0] - first[0], target[2] - 640);
                    destination[0] = Scale(gUnk_086149C4[(angle + 64) & 255] * 1024);
                    destination[1] = 0;
                    asm("" : "+r"(radius));
                    destination[2] = Scale(gUnk_086149C4[angle & 255] * radius);
                    AddVector_081A9178(destination, center, destination);
                    destination[1] = target[1];
                    MotionTrack_Initialize(track, p->position, destination, 0, 3);
                }
                p->timer++;
            }
            MotionTrack_Update(self->track);
            if (IsComplete(self)) {
                u8 next = 2;
                u32 zero = 0;
                *(&p->phase) = next;
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

#else
INCLUDE_ASM("asm/nonmatching", sub_081A9178);
#endif
