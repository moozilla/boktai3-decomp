#include "scene/collision/packed_contact_vector.h"
#include "effects/particles/particle_emission_vector_layout.h"
#define gSineTable gUnk_086149C4
#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
extern const s16 gUnk_086149C4[];
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ratatosk_OnFrontCollision(void);
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ParticleEmissionVector *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle & 255] * radius);
}
void Ratatosk_InitFrontCollider(struct Ratatosk *p)
{
    struct SceneActorBody *body = &p->body;
    struct ContactCollider *collider = &p->frontCollider;
    struct PackedContactVector size, offset;
    struct ParticleEmissionVector pos;
    size.x = 96;
    size.y = 128;
    size.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    polar(&pos, (u8)((body->direction << 6) - 64), 512);
    {
        struct ParticleEmissionVector *v = &pos;
        struct ContactPosition *base = &p->anchor;
        v->x = v->x + base->x;
        v->y = v->y + base->y;
        v->z = v->z + base->z;
    }
    Collider_SetPosition(collider, &pos, 0);
    Collider_SetCallback(collider, Ratatosk_OnFrontCollision, p);
    Collider_SetAttackProperties(collider, p->frontAttack[0], p->frontAttack[1], 0, 0x40000,
                                 p->frontAttack[2]);
}
