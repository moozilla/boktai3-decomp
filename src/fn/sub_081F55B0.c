#include "global.h"

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
extern const s16 gUnk_086149C4[];
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
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
void SceneParticles_SpawnAtPosition(void *, struct Vec *);
s32 Callback_InvokeActor(void *, void *);
// confidence=low: emits particles around the anchor during a timed pose.
#define Ratatosk_UpdateOrbitingParticlePose sub_081F55B0
void Ratatosk_UpdateOrbitingParticlePose(struct Actor *p)
{
    u8 params[0x18];
    struct Vec delta, spawn;
    u32 *timer;
    u32 *savedTimer;
    if (p->changed) {
        p->changed = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 31, 1, 4);
        Sound_PlaySong(0x56b);
    }
    spawn = p->anchor;
    timer = &p->timer;
    {
        s32 angle = (u8)(Mod(*timer, 31) << 3);
        s32 radius = 160;
        struct Vec *out = &delta;
        s32 product = gUnk_086149C4[(angle + 64) & 255] * radius;
        savedTimer = timer;
        out->x = trunc12(product);
        out->y = 0;
        out->z = trunc12(gUnk_086149C4[angle] * radius);
    }
    sum(&spawn, &spawn, &delta);
    spawn.y += 256;
    SlotMotion_Initialize(params);
    Ratatosk_ConfigureParticle(params, 20, 0, 0, 2, 0);
    SceneParticles_SpawnAtPosition(params, &spawn);
    if (*savedTimer > 5) {
        u32 mask = 0x800000;
        p->flags |= mask;
    }
    if (*savedTimer > 120) {
        Sound_StopSong(0x56b);
        p->finished = 1;
    }
    Callback_InvokeActor(p, p->callback);
}
