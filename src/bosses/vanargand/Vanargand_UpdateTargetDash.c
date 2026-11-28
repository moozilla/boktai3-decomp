#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

// confidence=low: starts a target-directed motion track and emits afterimages.
struct Vector {
    s16 x, y, z, pad;
};
extern const s16 gUnk_086149C4[];
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
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

static inline u8 IsComplete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}
static inline void SetPolar(struct Vector *v, s32 angle, s32 radius)
{
    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

void Vanargand_UpdateTargetDash(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    u32 mask = 0x48;
    u8 result;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;

    if (p->pending) {
        p->pending = result;
        p->phase = result;
        p->timer = result;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, &p->animation, p->drawable, 4, 1, 0);
                Sound_PlaySong(0x586);
                p->timer++;
            }
            if (p->action > 10) {
                p->phase = 1;
                p->timer = 0;
            }
            break;

        case 1:
            if (p->timer == 0) {
                void *track = &p->track;
                s16 *target = p->destination;
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
                    MotionTrack_Initialize(track, &p->position, target, 0, 3);
                } else {
                    SetPolar(&destination, Math_Atan2(target[0] - first[0], target[2] - 640), 1024);
                    AddVectorInPlace(&destination, &center);
                    destination.y = target[1];
                    MotionTrack_Initialize(track, &p->position, &destination, 0, 3);
                }
                p->timer++;
            }
            MotionTrack_Update(&self->track);
            if (IsComplete(self)) {
                u8 next = 2;
                p->phase = next;
                p->timer = 0;
            } else {
                VanargandAfterimagePool_Spawn(self->afterimages, &p->position);
            }
            break;

        case 2:
            p->finished = 1;
            break;
    }
}
