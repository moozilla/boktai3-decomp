#include "monsters/dragon_puppy.h"
s32 DragonPuppy_UpdateCombat(struct DragonPuppy *);
s32 DragonPuppy_OnTaskDestroy(u8 *);
s32 DragonPuppy_InitializeFromScript(void *, void *);
void *DragonPuppy_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct DragonPuppy));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))DragonPuppy_UpdateCombat,
                          (void (*)(void *))DragonPuppy_OnTaskDestroy);
        if (DragonPuppy_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
