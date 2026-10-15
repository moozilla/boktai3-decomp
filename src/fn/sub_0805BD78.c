#include "scene_panel_config.h"
#define ScenePanel_InitAuxiliarySprite sub_0805BD78
void ScenePanel_InitAuxiliarySprite(struct ScenePanel *input){
 struct ScenePanelConfigView *parent=(void*)input;
 struct ScenePanelAuxSprite *sprite=&parent->auxiliary;
 void *graphics=sprite->graphics;
 AnimData_Load(graphics,0x3641);Sprite_Init(sprite,graphics,512);
 sprite->frame=0;sprite->flags|=2;sprite->parameter9=96;sprite->parameter8=96;sprite->renderMode=1;
 sprite->position=parent->center;sprite->position.y+=72;
}
