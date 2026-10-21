#include "global.h"
#include "engine.h"
void *SceneActors_GetBySlot(u32 slot);
void SingletonSlot7_Update(void *);
void SingletonSlot7_OnDestroy(void *);
s32 SingletonSlot7_InitFromScript(void *, void *);
/* Registry routine 5856: returns the actor in slot 7 of the singleton table
   at 0x020000B8, or creates its task (0x630 bytes). */
void *SingletonSlot7_Create(void *arg)
{
    void *p = SceneActors_GetBySlot(7);
    if (p)
        return p;
    p = Task_Create(8, 0x630);
    if (p) {
        Task_SetCallbacks(p, SingletonSlot7_Update, SingletonSlot7_OnDestroy);
        if (SingletonSlot7_InitFromScript(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
