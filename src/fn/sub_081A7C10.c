#define ScenePanel_SelectPairTransition sub_081A7C10
#include "global.h"
struct ScenePanelPairSequence {
    u8 unknown0[0x5C4];
    u8 state, cursor, transition;
};
void Vanargand_UpdateModeEffect(struct ScenePanelPairSequence *);
void sub_081A7C10(struct ScenePanelPairSequence *panel, const u8 table[][3])
{
    do {
        panel->transition = table[panel->state][panel->cursor];
        panel->cursor++;
    } while (panel->transition == 0 && panel->cursor <= 2);
    if (panel->cursor > 2) {
        panel->cursor = 0;
        Vanargand_UpdateModeEffect(panel);
        if (panel->transition == 0)
            panel->transition = table[panel->state][panel->cursor];
    }
}
