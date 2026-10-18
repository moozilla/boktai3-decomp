#include "global.h"
// Provisional object-family interpretation.
#define SceneFade_BeginTargetMove sub_081C4CF0
struct SceneFade {
    u8 pad0[0x18]; void *target; s16 position[3]; u8 pad22[0x30-0x22];
    void (*callback)(void); u16 timer; u8 pad36[2]; s16 duration;
};
// Tentative: configures the target movement.
#define SceneTarget_BeginMove sub_081C32C8
void sub_081C32C8(void *, s16 *, s16 *, s32, s32, s32, s32);
void Camera_SetOffset(s32, u16 *);
void RegistryC281_UpdateFirstMovement(void);
void sub_081C4CF0(struct SceneFade *p)
{
    s16 a[3]; u16 b[3];
    s16 *q = a;
    s32 x = 0x500, y = 0x200, z = 0x868;
    s32 zero = 0;
    a[0] = x; q[1] = y; q[2] = z;
    sub_081C32C8(p->target, p->position, a, 0x30, 0x20, p->duration, 0x40);
    { u16 *r = b; s32 x = 0x768, y = 0x276;
    r[0] = x; r[1] = y; r[2] = z; }
    Camera_SetOffset(0xb4, b);
    p->callback = RegistryC281_UpdateFirstMovement;
    p->timer = zero;
}
