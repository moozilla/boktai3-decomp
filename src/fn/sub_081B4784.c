#define SceneParticleEmitter_EmitPeriodically sub_081B4784
#include "global.h"
struct SceneParticleEmitter {
    u8 unknown0[0x1c];
    s32 position[3];
    u8 unknown28[0x134];
    u32 variant;
    u32 flags;
    u8 unknown164[0xc];
    u8 active;
};
void OrbitingParticles_Create(void *,int,int,int,int);
void sub_081B4784(void *unused, struct SceneParticleEmitter *p, u32 frame)
{
    u32 mask = 12;
    p->flags |= mask;
    if ((frame & 31) == 0) {
        if (p->variant == 0)
            OrbitingParticles_Create(p->position,256,32,16,2);
        else
            OrbitingParticles_Create(p->position,256,32,16,3);
    }
    p->active = 1;
}
