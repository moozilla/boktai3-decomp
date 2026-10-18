#include "global.h"
// Provisional object-family interpretation.
#define SceneFade_BeginSecondMove sub_081C4D78
struct SceneFade {
    u8 pad0[0x18]; void *target; s16 position[3]; u8 pad22[0x30-0x22];
    void (*callback)(void); u16 timer; u8 pad36[4]; s16 duration;
};
// Tentative: configures the target movement.
#define SceneTarget_BeginMove sub_081C32C8
void sub_081C32C8(void *, s16 *, s16 *, s32, s32, s32, s32);
void RegistryC281_UpdateSecondMovement(void);
void sub_081C4D78(struct SceneFade *p)
{
    s16 a[3];
    s16 *q = a;
    s32 x = 0x668, y = 0x200, z = 0x768;
    s32 zero = 0;
    a[0] = x; q[1] = y; q[2] = z;
    sub_081C32C8(p->target, 0, q, 0x20, 0xc, p->duration, 0x3c);
    p->callback = RegistryC281_UpdateSecondMovement;
    p->timer = zero;
}
