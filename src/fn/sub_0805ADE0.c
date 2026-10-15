#include "scene_panel_rig_setup.h"
void ScenePanelRig_InitMainSprite(struct ScenePanelRig *rig)
{
    struct SceneRigSprite *sprite = (void *)&rig->main;
    void *graphics = sprite->graphics;
    void *resource;
    AnimData_Load(graphics, (u16)sub_08219BD8(gUnk_0824DEAC));
    Sprite_Init(sprite, graphics, 0);
    sprite->frame = 0;
    sprite->renderMode = 1;
    sprite->position = rig->position;
    resource = ScriptProc_Start(0x922E, (u16)sub_08219BD8(gUnk_0824DEB0));
    sprite->animationResource = resource;
    Anim_SetSequence(&sprite->sequence, resource, 0, 0, 0);
}
