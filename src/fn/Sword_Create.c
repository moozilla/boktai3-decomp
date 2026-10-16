#include "global.h"
#include "engine.h"
void sub_080F669C(void *);
void sub_080F6718(void *);
s32 sub_080F7458(void *);
/* Registry routine DB8D (Sword): creates a sword task (0xDA4 bytes) for the
   given id (stored at +0xD7C). */
void *Sword_Create(u16 id)
{
    u8 *p = Task_Create(8, 0xDA4);
    if (p) {
        *(u16 *)(p + 0xD7C) = id;
        Task_SetCallbacks(p, sub_080F669C, sub_080F6718);
        if (sub_080F7458(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
