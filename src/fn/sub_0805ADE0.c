#include "scene_panel_rig_setup.h"
#define ScenePanelRig_InitMainSprite sub_0805ADE0
void ScenePanelRig_InitMainSprite(struct ScenePanelRig *rig)
{
    struct SceneRigSetupSprite *sprite = (void *)&rig->main;
    void *graphics = sprite->graphics;
    void *resource;
    AnimData_Load(graphics, (u16)sub_08219BD8(gUnk_0824DEAC));
    Sprite_Init(sprite, graphics, 0);
    sprite->scale = 0;
    sprite->renderMode = 1;
    sprite->position = rig->position;
    resource = ScriptProc_Start(0x922E, (u16)sub_08219BD8(gUnk_0824DEB0));
    sprite->animationResource = resource;
    Anim_SetSequence(sprite->animation, resource, 0, 0, 0);
}
