#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
static inline u8 complete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}
static inline s16 *vector(s16 *p, s16 x, s16 y, s16 z)
{
    s16 *q = p;
    *q = x;
    p[1] = y;
    p[2] = z;
    return p;
}
static inline void initialize(void *track, void *data, s16 x, s16 y, s16 z, s16 *target, u32 mode)
{
    s16 origin[4];
    origin[0] = x;
    origin[1] = y;
    origin[2] = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}
void Vanargand_UpdateMotionTrack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *p2 = p;
    s16 target[4];
    u32 mask = 12;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, &p->animation, p->drawable, 2, 1, 4);
        vector(target, 8, 8, 8);
        initialize(&p->track, &p->position, 0x280, 0x300, 0x280, target, 5);
        PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 0x13);
    } else
        MotionTrack_Update(&p->track);
    if (complete(p2)) {
        if (PairedSceneActor_AreSelectedHalvesFinished(p2->pair)) {
            p2->mode = 0;
            p->finished = 1;
        }
    }
}
