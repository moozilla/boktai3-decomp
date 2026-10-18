#include "global.h"
// Provisional object-family interpretation.
struct SceneFade {
    u8 pad0[0x18];
    void *target;
    s16 position[3];
    u8 pad22[0x30 - 0x22];
    void (*callback)(void);
    u16 timer;
    u8 pad36[6];
    s16 duration;
};
// Tentative: configures the target movement.
void SceneTarget_BeginMove(void *, s16 *, s16 *, s32, s32, s32, s32);
void RegistryC281_UpdateFinalMovement(void);
void SceneFade_BeginFinalMove(struct SceneFade *p)
{
    s16 a[3];
    s16 *q = a;
    s32 x = 0x580, y = 0x200, z = 0x868;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = z;
    SceneTarget_BeginMove(p->target, 0, q, 0xc, 1, p->duration, 0x3c);
    p->callback = RegistryC281_UpdateFinalMovement;
    p->timer = zero;
}
