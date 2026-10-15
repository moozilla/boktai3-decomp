#include "scene_panel_config.h"
#define ScenePanel_AreSubjectsIdle sub_0805C2DC
s32 ScenePanel_AreSubjectsIdle(struct ScenePanel *input){
 struct ScenePanelConfigView *parent=(void*)input;
 s32 i=0,count=parent->subjectCount;
 if(i<count){s32 limit=count;struct SceneRigSubject **subject=parent->subjects;do{if((*subject)->state!=0)return 0;subject++;i++;}while(i<limit);}
 return 1;
}
