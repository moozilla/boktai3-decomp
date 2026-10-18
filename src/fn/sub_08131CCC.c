#define SceneActorBehavior_InstallCallbackTables sub_08131CCC
#include "global.h"
#include "scene_actor_reset.h"
#define gSceneActorCallbacks27C gUnk_0861081C
extern void *gSceneActorCallbacks27C[];
#define gSceneActorCallbacks280 gUnk_0861082C
extern void *gSceneActorCallbacks280[];
#define gSceneActorCallbacks284 gUnk_08610838
extern void *gSceneActorCallbacks284[];
#define gSceneActorCallbacks288 gUnk_08610874
extern void *gSceneActorCallbacks288[];
#define gSceneActorCallbacks28C gUnk_08610878
extern void *gSceneActorCallbacks28C[];
#define gSceneActorCallbacks290 gUnk_0861087C
extern void *gSceneActorCallbacks290[];
#define gSceneActorCallbacks294 gUnk_08610880
extern void *gSceneActorCallbacks294[];
#define gSceneActorCallbacks298 gUnk_0861088C
extern void *gSceneActorCallbacks298[];
#define gSceneActorCallbacks29C gUnk_08610890
extern void *gSceneActorCallbacks29C[];
#define gSceneActorCallbacks2A0 gUnk_086108A8
extern void *gSceneActorCallbacks2A0[];
void sub_08131CCC(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks27C = gSceneActorCallbacks27C;
    actor->callbacks280 = gSceneActorCallbacks280;
    actor->callbacks284 = gSceneActorCallbacks284;
    actor->alternateBehaviorCallbacks = gSceneActorCallbacks288;
    actor->behaviorCallbacks = gSceneActorCallbacks28C;
    actor->callbacks290 = gSceneActorCallbacks290;
    actor->callbacks294 = gSceneActorCallbacks294;
    actor->phaseCallbacks = gSceneActorCallbacks298;
    actor->callbacks29C = gSceneActorCallbacks29C;
    actor->callbacks2A0 = gSceneActorCallbacks2A0;
}
