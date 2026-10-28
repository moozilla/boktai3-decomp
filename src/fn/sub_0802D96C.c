#include "global.h"
#include "ram.h"
struct Object {
    u8 active;
    u8 pad1[0x5b];
    u8 motion[0x78];
    u8 drawable[1];
};
void Actor_UnlinkFromManager(void *, struct Object *);
void Actor_UnregisterMotion(struct Object *);
void SceneBody_Unregister(void *);
void DisplayAnimation_Destroy(void *);
void Actor_DestroyShadow(struct Object *);
void Heap_Free(void *);
int Actor_Destroy(struct Object *p)
{
    int result;
    if (gSceneActorListManager && p->active) {
        Actor_UnlinkFromManager(gSceneActorListManager, p);
        Actor_UnregisterMotion(p);
        SceneBody_Unregister(p->motion);
        DisplayAnimation_Destroy(p->drawable);
        Actor_DestroyShadow(p);
        p->active = 0;
        Heap_Free(p);
        result = 0;
    } else
        result = -1;
    return result;
}
