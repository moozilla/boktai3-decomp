#include "global.h"
#include "engine/ram.h"
/* ScreenOrbitEffect_EnsureSingleton: Returns the existing effect manager or creates and initializes its task */
u8 *Task_Create(s32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void EffectQuad_UpdateEntries(u8 *);
void ScreenOrbitEffect_Destroy(u8 *);
s32 ScreenOrbitEffect_Init(u8 *);
void Task_Kill(u8 *);

u8 *ScreenOrbitEffect_EnsureSingleton(void)
{
    u8 *p;
    if (gEffectQuad != NULL)
        return gEffectQuad;
    p = Task_Create(11, 0x104);
    if (p != NULL) {
        Task_SetCallbacks(p, EffectQuad_UpdateEntries, ScreenOrbitEffect_Destroy);
        if (ScreenOrbitEffect_Init(p) < 0) {
            Task_Kill(p);
            return NULL;
        }
    }
    return p;
}
