// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyConsumableMove(struct SceneInventoryControlView *owner)
{
    register struct SceneInventoryControlView *incomingPanel asm("r5") = owner;
    struct SceneInventoryControlView *panel;
    register u32 incoming asm("r1");
    panel = incomingPanel;
    incoming = panel->grid.savedSelection;
    if (incoming <= 15) {
        s32 saved = incoming;
        s32 first;
        register s32 selected asm("r1");
        s32 second;
        {
            struct GameState *state = gGameState;
            register struct GameState *slot asm("r0") = (void *)(saved * 2 + (u32)state);
            first = slot->table160[0];
            selected = panel->grid.selection;
            slot = (void *)(selected * 2 + (u32)state);
            second = slot->table160[0];
        }
        if (saved == selected)
            return 0;
        if (first < 0 && second < 0)
            return 0;
        if (panel->mode == 1)
            return 1;
    } else {
        register u32 selectionOffset asm("r0") = 0x1BE7;
        register u8 *selectedPtr asm("r2");
        u32 selected;
        asm("" : "+r"(selectionOffset));
        selectedPtr = (void *)((u32)panel + selectionOffset);
        selected = *selectedPtr;
        if (selected == incoming) {
            SceneInventory_SelectActionDestination(panel);
            return 4;
        }
        if (selected <= 15) {
            s32 second;
            {
                struct GameState *slot;
                {
                    u32 index = *selectedPtr;
                    struct GameState *state;
                    state = gGameState;
                    slot = (void *)(index * 2 + (u32)state);
                }
                {
                    u32 memberOffset = 0x160;
                    asm("" : "+r"(memberOffset));
                    second = *(s16 *)((u32)slot + memberOffset);
                }
            }
            if (second < 0)
                return 4;
        }
    }
    return 0;
}
