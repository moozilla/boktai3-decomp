#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
s32 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
static inline s16 *vector(s16 *p, s16 x, s16 y, s16 z)
{
    s16 *q = p;
    *q = x;
    p[1] = y;
    p[2] = z;
    return p;
}
static inline u8 complete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}
void Vanargand_UpdateMotionReturn(struct VanargandPairAttack *p)
{
    void *animation = &p->animation;
    u8 result;
    struct MotionTrackVector origin;
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pending) {
        p->pending = result;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 2, 1, 4);
        vector(&origin.x, 0x300, 0x200, 0x300);
        MotionTrack_Initialize(&p->track, &p->position, &origin, 0, 4);
    }
    MotionTrack_Update(&p->track);
    if (complete(p))
        p->finished = 1;
}
