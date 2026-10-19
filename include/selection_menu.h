#ifndef GUARD_SELECTION_MENU_H
#define GUARD_SELECTION_MENU_H
#include "scene_input_panel.h"

/* Tentative actor/item combination menu, allocated as 0xDB4 bytes. */
struct SelectionMenu {
    struct Task task;
    u8 unknown18[4];
    u8 callbackPending, selectedSlot, choiceCount, repeatTicks;
    u8 horizontalChoice, choices[2], unknown23, statisticsPage;
    u8 unknown25[3];
    u32 frame;
    u8 unknown2C[8];
    u32 availableSlots;
    struct ContactPosition choiceTarget;
    u16 cancelScript, confirmScript;
    u8 graphicData[8];
    struct SceneInputResource panelResource, iconResource;
    struct SceneInputPanelDrawable panel;
    struct SceneInputWidget panels[2], cursors[3], markers[8];
    struct SceneInputWidget highlight, heading, icons[19];
    u8 result[16];
    void (*callback)(struct SelectionMenu *);
    u32 actorMessages, itemMessages, promptMessages;
};
/* Packed copy preserves the original paired-word position copy; the z/pad
 * components are not initialized by the grid population routines. */
struct SelectionMenuPackedPosition {
    signed x : 16, y : 16, z : 16, pad : 16;
};
struct SelectionMenuScriptValues {
    u32 actor, item;
};
struct SelectionMenuScriptCall {
    u32 flags : 16, unused : 16;
    struct SelectionMenuScriptValues *args;
};
struct SelectionMenuItem {
    u8 graphic, unknown1[9], available, unknownB[5];
};
struct SelectionMenuActor {
    u8 unknown0[2], level, unknown3[21];
    u32 restrictions[2];
};
extern struct SelectionMenuItem gUnk_08E60F80[];
extern struct SelectionMenuActor gUnk_08E6092C[];
#endif
