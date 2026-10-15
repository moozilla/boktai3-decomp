#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_Create sub_0805DCAC
struct ScenePanelSubjectView *ScenePanelSubject_Create(const struct ContactPosition *home,u32 row,u32 variant,u32 state,u32 kind,u32 contactLimit,u32 colliderSetting,u32 appearDelay){
 struct ScenePanelSubjectView *subject=Task_Create(8,0x240);
 if(subject){
 Task_SetCallbacks(subject,(void(*)(void*))sub_0805D800,(void(*)(void*))sub_0805D884);
 if(sub_0805D92C(subject,home,row,variant,state,kind,contactLimit,colliderSetting,appearDelay)<0){Task_Kill(subject);return 0;}
 }
 return subject;
}
