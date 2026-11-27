#include "monsters/popbud/popbud_playback.h"
s32 Popbud_UpdateDamageAnimation(struct PinkSpineState *);
s32 Popbud_OnTaskDestroy(u8 *);
s32 Popbud_InitFromScript(void *, void *);
void *Popbud_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct PinkSpineState));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))Popbud_UpdateDamageAnimation,
                          (void (*)(void *))Popbud_OnTaskDestroy);
        if (Popbud_InitFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
