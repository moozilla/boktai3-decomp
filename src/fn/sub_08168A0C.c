// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
#include "player.h"

/* Four primary records occupy the space before the primary widgets. */
struct PrimaryOption {
    u8 value, icon;
    u8 unk_02[2];
};
struct PrimaryOptionsView {
    u8 unk_00[0xC78];
    struct PrimaryOption options[4];
};

void SceneHud_RebuildPrimaryOptions(struct SceneHudState *p)
{
    /* Keep the shared-state reads at the point where each choice is tested. */
    struct GameState *volatile *gameState;
    volatile u32 *flags;
    s32 count = 0;
    s32 i;
    s32 j;
    struct PrimaryOption *option;

    p->primarySelection = -1;
    if (p->actionOwner->actionMode != 6) {
        register struct GameState *state asm("r3");
        s16 *slots;
        s16 **output;
        s8 *selection;
        output = &p->primarySelectionOutput;
        state = gGameState;
        *output = &state->selected;
        i = 0;
        gameState = &gGameState;
        flags = &gEngineFlags;
        selection = &p->primarySelection;
        slots = state->slots88;
        option = ((struct PrimaryOptionsView *)p)->options;
        for (; i < 4; i++) {
            if (*slots >= 0) {
                option->value = i;
                option->icon = *slots;
                if (i == *p->primarySelectionOutput)
                    *selection = count;
                option++;
                count++;
            }
            slots++;
        }
    } else {
        p->primarySelectionOutput = &gGameState->abilityVariant;
        i = 0;
        gameState = &gGameState;
        flags = &gEngineFlags;
        for (; i < 2; i++) {
            option = &((struct PrimaryOptionsView *)p)->options[count];
            option->value = i;
            if (i == 0)
                option->icon = 4;
            else
                option->icon = 5;
            if (i == *p->primarySelectionOutput)
                p->primarySelection = count;
            count++;
        }
    }
    p->primaryVisible = count;
    for (; count < 4; count++) {
        option = &((struct PrimaryOptionsView *)p)->options[count];
        option->value |= 0xFF;
        option->icon |= 0xFF;
    }
    for (i = 3; i >= 0; i--) {
    }
    if ((*flags & 0x1000) && p->primarySelection < 0) {
        *p->primarySelectionOutput = (s8)((struct PrimaryOptionsView *)p)->options[0].value;
        p->primarySelection = 0;
    }
    if (p->primaryVisible && p->primarySelection < 0) {
        for (i = 1; i < 4; i++) {
            s32 slot = (*p->primarySelectionOutput + i) & 3;
            struct GameState *state = *gameState;
            /* Index before the table offset, preserving the slot-table address order. */
            u32 address = slot * 2 + (u32)state;
            if (((struct GameState *)address)->slots88[0] >= 0) {
                *p->primarySelectionOutput = (*p->primarySelectionOutput + i) & 3;
                for (j = 0; j < 4; j++) {
                    if ((s8)((struct PrimaryOptionsView *)p)->options[j].value ==
                        *p->primarySelectionOutput)
                        p->primarySelection = j;
                }
                break;
            }
        }
    }
    p->primaryBlinkTimer = 0;
}
