#include "global.h"
#include "engine.h"
extern void *gSceneActorContactManager;
void SceneActorContacts_Update(void *);
void SceneActorContacts_Destroy(void *);
s32 SceneActorContacts_SetOwner(void *, u16);
/* Registry routine D854: returns the scene actor contact manager
   (gSceneActorContactManager), or creates it
   (0x2C-byte task) for the given id. */
void *SceneActorContacts_Create(u16 id)
{
    void *p;
    if (gSceneActorContactManager)
        return gSceneActorContactManager;
    p = Task_Create(8, 0x2C);
    if (p) {
        Task_SetCallbacks(p, SceneActorContacts_Update, SceneActorContacts_Destroy);
        if (SceneActorContacts_SetOwner(p, id) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
