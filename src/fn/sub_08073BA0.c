#include "global.h"
#include "engine.h"
extern void *gUnk_020001A0;
void SceneActorContacts_Update(void *);
void SceneActorContacts_Destroy(void *);
s32 sub_08073B84(void *, u16);
/* Registry routine D854: returns the scene actor contact manager
   (gUnk_020001A0), or creates it
   (0x2C-byte task) for the given id. */
void *SceneActorContacts_Create(u16 id)
{
    void *p;
    if (gUnk_020001A0)
        return gUnk_020001A0;
    p = Task_Create(8, 0x2C);
    if (p) {
        Task_SetCallbacks(p, SceneActorContacts_Update, SceneActorContacts_Destroy);
        if (sub_08073B84(p, id) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
