#include "global.h"
#include "system/game_state/game_state.h"

s32 Div(s32, s32);

s32 GameStats_SelectLeadingCounter(void)
{
    s32 values[5];
    s32 i;
    s32 maximum;
    s32 selected;

    for (i = 0; i <= 4; i++)
        values[i] = Div(*(i + gGameState->counters640), 54000);
    maximum = 0;
    selected = 5;
    for (i = 0; i <= 4; i++) {
        if (values[i] > maximum) {
            maximum = values[i];
            selected = i;
        } else if (values[i] == maximum) {
            selected = 5;
        }
    }
    gGameState->unk662 = selected;
    return selected;
}
