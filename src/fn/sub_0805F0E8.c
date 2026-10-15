// MATCHING_CONCESSION: register-pinning
#include "scene_input_panel.h"
void SceneInputPanel_PositionModeCursor(struct SceneInputPanel *owner, u32 mode)
{
    register struct SceneInputPanel *panel asm("r2") = owner;
    struct SceneInputPanel *base;
    base = panel;
    switch (mode) {
        case 4:
        case 5: {
            register struct SceneInputWidget *cursor asm("r2") = &base->modeCursor;
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 32;
            break;
        case 1: {
            register u32 offset asm("r1") = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            register struct SceneInputWidget *cursor asm("r2");
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 56;
            break;
        case 3: {
            register u32 offset asm("r1") = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            register struct SceneInputWidget *cursor asm("r2");
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 80;
            break;
        case 2: {
            register u32 offset asm("r1") = (u32) & ((struct SceneInputPanel *)0)->modeCursor;
            register struct SceneInputWidget *cursor asm("r2");
            cursor = (void *)((u8 *)base + offset);
            cursor->flags &= ~1;
        }
            base->modeCursor.position.x = 216;
            base->modeCursor.position.y = 104;
            break;
        default: {
            register struct SceneInputWidget *cursor asm("r0") = &panel->modeCursor;
            cursor->flags |= 1;
        } break;
    }
}
