#include "engine/shared/global.h"
#include "menus/scene_hud/scene_hud_display.h"
#include "system/game_state/game_state.h"
#include "engine/shared/player.h"
#include "engine/shared/debug.h"

void SceneHud_RebuildPrimaryOptions(struct SceneHudState *p)
{
    s32 count = 0;
    s32 i;
    s32 j;
    struct SceneHudPrimaryOption *option;

    p->primarySelection = -1;
    if (p->actionOwner->actionMode != 6) {
        struct GameState *state;
        s16 *slots;
        s16 **output = &p->primarySelectionOutput;
        state = gGameState;
        slots = state->slots88;
        *output = &state->selected;
        for (i = 0; i < 4; i++) {
            if (slots[i] >= 0) {
                option = &p->primaryOptions[count];
                option->value = i;
                option->icon = slots[i];
                if (i == *p->primarySelectionOutput)
                    p->primarySelection = count;
                count++;
            }
        }
    } else {
        p->primarySelectionOutput = &gGameState->abilityVariant;
        i = 0;
        for (; i < 2; i++) {
            option = &p->primaryOptions[count];
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
        option = &p->primaryOptions[count];
        option->value |= 0xFFu;
        option->icon |= 0xFFu;
    }
    for (i = 3; i >= 0; i--) {
        DEBUG_PRINT();
    }
    if ((gEngineFlags & 0x1000) && p->primarySelection < 0) {
        *p->primarySelectionOutput = (s8)p->primaryOptions[0].value;
        p->primarySelection = 0;
    }
    if (p->primaryVisible && p->primarySelection < 0) {
        for (i = 1; i < 4; i++) {
            s32 slot = (*p->primarySelectionOutput + i) & 3;
            if (*(gGameState->slots88 + slot) >= 0) {
                *p->primarySelectionOutput = (*p->primarySelectionOutput + i) & 3;
                for (j = 0; j < 4; j++) {
                    if ((s8)p->primaryOptions[j].value == *p->primarySelectionOutput)
                        p->primarySelection = j;
                }
                break;
            }
        }
    }
    p->primaryBlinkTimer = 0;
}
