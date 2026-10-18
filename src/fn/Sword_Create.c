#include "global.h"
#include "engine.h"
void Sword_UpdateTask(void *);
void Sword_DestroyTask(void *);
s32 sub_080F7458(void *);
/* Registry routine DB8D (Sword): creates a sword task (0xDA4 bytes) for the
   given id (stored at +0xD7C). */
void *Sword_Create(u16 id)
{
    u8 *p = Task_Create(8, 0xDA4);
    if (p) {
        *(u16 *)(p + 0xD7C) = id;
        Task_SetCallbacks(p, Sword_UpdateTask, Sword_DestroyTask);
        if (sub_080F7458(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
