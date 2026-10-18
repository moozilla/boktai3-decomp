#include "global.h"

struct S {
    u8 f[0x58];
    u32 a;
};
void OrbitingParticles_Stop(u32);
void SceneCommands_ClearEffect(struct S *p)
{
    if (p->a != 0) {
        OrbitingParticles_Stop(p->a);
        p->a = 0;
    }
}
