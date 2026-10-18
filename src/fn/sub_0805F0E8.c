#include "scene_input_panel.h"
void SceneInputPanel_PositionModeCursor(struct SceneInputPanel *owner, u32 mode)
{
    struct SceneInputPanel *panel = owner;
    struct SceneInputPanel *base;
    base = panel;
    switch (mode) {
        case 4:
        case 5: {
            struct SceneInputWidget *cursor = &base->modeCursor;
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 32;
            break;
        case 1: {
            u32 offset = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            struct SceneInputWidget *cursor;
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 56;
            break;
        case 3: {
            u32 offset = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            struct SceneInputWidget *cursor;
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 80;
            break;
        case 2: {
            u32 offset = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            struct SceneInputWidget *cursor;
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 104;
            break;
        default: {
            struct SceneInputWidget *cursor = &panel->modeCursor;
            cursor->flags |= 1;
        } break;
    }
}
