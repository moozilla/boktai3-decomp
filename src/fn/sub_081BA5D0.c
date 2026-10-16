#include "global.h"
#include "engine.h"
#define Registry2388_Create sub_081BA5D0
extern void *gUnk_0200025C;
void sub_081BA27C(void *);
void sub_081BA2A4(void *);
s32 sub_081BA3D4(void *, void *);
/* Registry routine 2388: creates its task (0x564 bytes) once and records it in
   gUnk_0200025C; returns 0 if it already exists. */
void *Registry2388_Create(void *arg)
{
    void *p;
    if (gUnk_0200025C)
        return 0;
    p = Task_Create(8, 0x564);
    if (p) {
        Task_SetCallbacks(p, sub_081BA27C, sub_081BA2A4);
        if (sub_081BA3D4(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_0200025C = p;
    return p;
}
