#include "global.h"
#include "game_state.h"

s32 Div(s32, s32);

s32 GameStats_SelectLeadingCounter(void)
{
    s32 values[5];
    s32 i;
    s32 maximum;
    s32 selected;

    /* Apply the element displacement before the field offset, as in the ROM. */
    for (i = 0; i <= 4; i++)
        values[i] = Div(((struct GameState *)((u8 *)gGameState + i * 4))->counters640[0], 54000);
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
    *(s16 *)((u8 *)gGameState + 0x662) = selected;
    return selected;
}
