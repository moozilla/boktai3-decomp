#include "scene/collision/contact_position.h"
#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
extern s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void SlotMotion_Initialize(u8 *);
int SceneParticles_SpawnAtPosition(u8 *, void *);
void Ratatosk_ConfigureParticle(u8 *, u16, u32, u8, u32, u32);
static inline int trunc12(int n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void add(struct ContactPosition *a, struct ContactPosition *b)
{
    b->x += a->x;
    b->y += a->y;
    b->z += a->z;
}
static inline void polar(s32 angle, s32 radius, struct ContactPosition *v)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

void SceneActors_UpdateOrbitingParticleAnimation(struct Ratatosk *p)
{
    u8 particle[0x18];
    struct ContactPosition delta;
    struct ContactPosition pos;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 31, 1, 4);
    }
    pos = p->anchor;
    polar((u8)(Mod(p->callbackFrame, 31) << 3), 160, &delta);
    add(&delta, &pos);
    pos.y += 256;
    SlotMotion_Initialize(particle);
    Ratatosk_ConfigureParticle(particle, 20, 0, 0, 2, 0);
    SceneParticles_SpawnAtPosition(particle, &pos);
    if (p->callbackFrame > 60)
        p->actionFinished = 1;
}
