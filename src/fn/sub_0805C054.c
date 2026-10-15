#include "scene_panel_config.h"
#define ScenePanel_InitScriptProperty sub_0805C054
void ScenePanel_InitScriptProperty(struct ScenePanel *input){
 struct ScenePanelConfigView *parent=(void*)input;
 s32 value=Script_SeekToKeyword('p');if(value)parent->property=Script_GetValue();else parent->property=value;
}
