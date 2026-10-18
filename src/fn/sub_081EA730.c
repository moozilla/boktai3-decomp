#include "global.h"
/* Create the singleton animated controller and install its task callbacks. */
struct S {
    u8 pad[0xf8a];
    u16 resource;
};
extern struct S *gUnk_020003C8;
struct S *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Actor_UpdateController(void *);
void Actor_DestroyController(void *);
s32 sub_081EA4DC(void *);
void Task_Kill(void *);
struct S *Actor_EnsureController(u32 resource, u32 unused)
{
    struct S *p;
    if (gUnk_020003C8)
        return gUnk_020003C8;
    p = Task_Create(11, 0xfa4);
    if (p) {
        p->resource = resource;
        Task_SetCallbacks(p, Actor_UpdateController, Actor_DestroyController);
        if (sub_081EA4DC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
