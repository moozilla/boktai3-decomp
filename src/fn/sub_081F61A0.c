#define SceneActors_UpdateOrbitingParticleAnimation sub_081F61A0
#define SceneParticles_InitParameters               sub_081FBDE0
#define SceneParticles_SpawnAtPosition              sub_081FBBA8
#define gSceneAngleSineTable                        gUnk_086149C4
#include "global.h"
typedef struct SceneParticleActor {
    u8 pad0[0x90];
    u32 tick;
    u8 pad94[0x1b];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 padB2[0x2ba];
    struct {
        s16 x, y, z, pad;
    } position;
} SceneParticleActor;
typedef struct ParticlePosition {
    s16 x, y, z, pad;
} ParticlePosition;
extern s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void sub_081FBDE0(u8 *);
int sub_081FBBA8(u8 *, void *);
void Ratatosk_ConfigureParticle(u8 *, u16, u32, u8, u32, u32);
static inline int trunc12(int n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void add(ParticlePosition *a, ParticlePosition *b)
{
    b->x += a->x;
    b->y += a->y;
    b->z += a->z;
}
void sub_081F61A0(SceneParticleActor *p)
{
    u8 particle[0x18];
    ParticlePosition delta;
    ParticlePosition pos;
    u32 angle;
    int speed;
    u32 zero;
    s16 *velocity;
    if (p->changed) {
        p->changed = 0;
        SceneActors_StartUnmirroredAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 31, 1, 4);
    }
    pos = *(ParticlePosition *)&p->position;
    angle = (u8)(Mod(p->tick, 31) << 3);
    speed = 160;
    velocity = (s16 *)&delta;
    {
        int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
        int zero = 0;
        velocity[0] = x;
        velocity[1] = zero;
    }
    {
        int z = trunc12(gUnk_086149C4[angle] * speed);
        zero = 0;
        velocity[2] = z;
    }
    add(&delta, &pos);
    pos.y += 256;
    sub_081FBDE0(particle);
    Ratatosk_ConfigureParticle(particle, 20, zero, zero, 2, zero);
    sub_081FBBA8(particle, &pos);
    if (p->tick > 60)
        p->finished = 1;
}
