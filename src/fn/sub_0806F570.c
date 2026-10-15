#include "scene_effect_schedule.h"
#define SceneEffectSchedule_Create sub_0806F570
struct SceneEffectSchedule *SceneEffectSchedule_Create(u32 argument)
{
 struct SceneEffectSchedule *manager;
 if(gUnk_0200019C)return gUnk_0200019C;
 manager=Task_Create(10,sizeof(*manager));
 if(manager){
 Task_SetCallbacks(manager,(void (*)(void *))sub_0806F448,(void (*)(void *))sub_0806F54C);
 if(sub_0806F558(manager,argument)<0){Task_Kill(manager);return 0;}
 }
 return manager;
}
