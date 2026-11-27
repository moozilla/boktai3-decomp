#include "monsters/garmr_statue.h"
s32 GarmrStatue_UpdateCombat(struct GarmrStatue *);
s32 GarmrStatue_OnTaskDestroy(u8 *);
s32 GarmrStatue_InitializeFromScript(void *, void *);
void *GarmrStatue_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct GarmrStatue));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))GarmrStatue_UpdateCombat,
                          (void (*)(void *))GarmrStatue_OnTaskDestroy);
        if (GarmrStatue_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
