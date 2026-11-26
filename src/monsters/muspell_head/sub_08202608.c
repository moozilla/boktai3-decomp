#include "monsters/muspell_head.h"
u32 MuspellHead_UpdateState(struct MuspellHead *);
s32 MuspellHead_Destroy(u8 *);
s32 MuspellHead_InitializeFromScript(void *, void *);
void *MuspellHead_CreateFromScript(void *p)
{
    void *r = Task_Create(8, sizeof(struct MuspellHead));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))MuspellHead_UpdateState,
                          (void (*)(void *))MuspellHead_Destroy);
        if (MuspellHead_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
