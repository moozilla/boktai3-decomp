#include "global.h"
#include "engine.h"
extern void *gUnk_0200025C;
void Registry2388_OnUpdate(void *);
void SelectionPanel_Destroy(void *);
s32 Registry2388_InitializeFromScript(void *, void *);
/* Registry routine 2388: creates its task (0x564 bytes) once and records it in
   gUnk_0200025C; returns 0 if it already exists. */
void *Registry2388_Create(void *arg)
{
    void *p;
    if (gUnk_0200025C)
        return 0;
    p = Task_Create(8, 0x564);
    if (p) {
        Task_SetCallbacks(p, Registry2388_OnUpdate, SelectionPanel_Destroy);
        if (Registry2388_InitializeFromScript(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_0200025C = p;
    return p;
}
