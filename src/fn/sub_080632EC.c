#include "contact_swarm.h"
#include "scene_input_panel.h"
#include "scene_input_grid.h"
/* The cleanup span contains 54 panel widgets and 15 comparison widgets. */
struct SceneInventoryCleanupView {
    u8 task[0x60];
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct SceneInputWidget widgets[69];
    u8 unknown1A88[0x1AF0 - 0x1A88];
    struct SceneInputPair transition;
    u8 unknown1BE4[0x30];
    struct SceneInputPair prompt;
};
void SceneInput_DestroyMarkerPair(struct SceneInputPair *);
void SceneInput_DestroyGlyphPair(struct SceneInputPair *);
void sub_08030BF8(void);
void sub_08033468(void);
s32 SceneInventory_DestroyController(struct SceneInventoryCleanupView *owner)
{
    struct SceneInputWidget *widget;
    s32 i;
    Sprite_Remove(&owner->sprite);
    widget = owner->widgets;
    i = 68;
    do {
        Anim_Remove(widget);
        widget++;
        i--;
    } while (i >= 0);
    SceneInput_DestroyMarkerPair(&owner->transition);
    SceneInput_DestroyGlyphPair(&owner->prompt);
    sub_08030BF8();
    sub_08033468();
    return 0;
}
