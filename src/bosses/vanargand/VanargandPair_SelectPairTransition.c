#include "global.h"
#include "bosses/vanargand/vanargand_pair_state_layout.h"
void Vanargand_UpdateModeEffect(struct VanargandPairState *);
void VanargandPair_SelectPairTransition(struct VanargandPairState *panel, const u8 table[][3])
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
