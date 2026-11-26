#include "monsters/ooze.h"
s32 SceneTask_DispatchUnlessPaused(struct Ooze *);
s32 Ooze_Destroy(u8 *);
s32 Ooze_InitializeFromScript(void *, void *);
void *Ooze_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct Ooze));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))SceneTask_DispatchUnlessPaused,
                          (void (*)(void *))Ooze_Destroy);
        if (Ooze_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
