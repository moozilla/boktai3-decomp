#include "global.h"
// Provisional object-family interpretation.
struct SceneFade {
    u8 pad[0x30];
    void (*callback)(void);
    u16 timer;
};
void Camera_SetOffset(s32, u16 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
void RegistryC281_UpdateOpeningDelay(void);
void SceneFade_BeginSoundFade(struct SceneFade *p)
{
    u16 a[3];
    u16 *q = a;
    s32 x = 0x868, y = 0x276;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = x;
    Camera_SetOffset(0x3c, a);
    ScreenTransition_SendCommand(0, 4, 0x1f, 0x1f, 0x1f, 0xffff, zero);
    Sound_PlaySong(0x47e);
    p->callback = RegistryC281_UpdateOpeningDelay;
    p->timer = zero;
}
