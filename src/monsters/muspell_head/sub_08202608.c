#include "monsters/muspell_head.h"
u32 sub_08201D38(struct MuspellHead *);
s32 MuspellHead_Destroy(u8 *);
s32 sub_0820212C(void *, void *);
void *MuspellHead_CreateFromScript(void *p)
{
    void *r = Task_Create(8, sizeof(struct MuspellHead));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))sub_08201D38,
                          (void (*)(void *))MuspellHead_Destroy);
        if (sub_0820212C(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
