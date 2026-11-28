#include "ui/widgets/scene_input_panel.h"
void SceneInputPanel_PositionModeCursor(struct SceneInputPanel *panel, u32 mode)
{
    struct SceneInputPanel *base = panel;

    switch (mode) {
        case 4:
        case 5:
            AnimSprite_Show(&base->modeCursor);
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 32;
            break;
        case 1:
            AnimSprite_Show(&base->modeCursor);
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 56;
            break;
        case 3:
            AnimSprite_Show(&base->modeCursor);
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 80;
            break;
        case 2:
            AnimSprite_Show(&base->modeCursor);
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 104;
            break;
        default:
            AnimSprite_Hide(&panel->modeCursor);
            break;
    }
}
