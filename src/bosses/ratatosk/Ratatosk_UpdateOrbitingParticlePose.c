#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern const s16 gUnk_086149C4[];
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(s32 angle, s32 radius, struct ContactPosition *v)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct ContactPosition *d, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
s32 Mod(s32, s32);
void SlotMotion_Initialize(void *);
void Ratatosk_ConfigureParticle(void *, s32, s32, s32, s32, s32);
void SceneParticles_SpawnAtPosition(void *, struct ContactPosition *);
s32 Callback_InvokeActor(void *, void *);
// confidence=low: emits particles around the anchor during a timed pose.
void Ratatosk_UpdateOrbitingParticlePose(struct Ratatosk *p)
{
    u8 params[0x18];
    struct ContactPosition delta, spawn;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 31, 1, 4);
        Sound_PlaySong(0x56b);
    }
    spawn = p->anchor;
    polar((u8)(Mod(p->callbackFrame, 31) << 3), 160, &delta);
    sum(&spawn, &spawn, &delta);
    spawn.y += 256;
    SlotMotion_Initialize(params);
    Ratatosk_ConfigureParticle(params, 20, 0, 0, 2, 0);
    SceneParticles_SpawnAtPosition(params, &spawn);
    if (p->callbackFrame > 5) {
        Ratatosk_SetFlags(p, 0x800000);
    }
    if (p->callbackFrame > 120) {
        Sound_StopSong(0x56b);
        p->actionFinished = 1;
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}
