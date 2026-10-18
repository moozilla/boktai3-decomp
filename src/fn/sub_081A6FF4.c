#include "global.h"
/* Tentative ScenePanel family; names describe observed state and sprite operations. */

struct ScenePanelAnimation {
    u8 pad0[6];
    u16 tile;
    u8 pad8[4];
    void *tiles;
    u8 pad10[12];
};
struct ScenePanelAuxiliarySprite {
    u8 pad0[7];
    u8 mode;
    u8 pad8[8];
    u16 priority;
    u8 pad12[10];
    u16 scale[3];
    u8 pad22[10];
    struct ScenePanelAnimation animation;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void ScenePanel_InitAuxiliarySprite_081A6FF4(u8 *panel, struct ScenePanelAuxiliarySprite *sprite)
{
    struct ScenePanelAnimation *animation = &sprite->animation;
    u16 *scale;
    u32 width, height;
    AnimData_Load(animation, 0x89CC);
    animation->tile = 0x6F;
    animation->tiles = panel + 0x718;
    Sprite_Init(sprite, animation, 0);
    sprite->priority = 5;
    sprite->mode = 3;
    scale = sprite->scale;
    width = 0x33B;
    height = 0x100;
    sprite->scale[0] = width;
    scale[1] = height;
    scale[2] = width;
}
