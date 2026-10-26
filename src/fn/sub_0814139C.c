#include "global.h"
#include "player.h"
#include "game_state.h"
void Player_CallEventScript(struct Player *, u32);
void Player_UpdateTimedScriptEvent(struct Player *p)
{
    u32 index = p->timedEventSlot;
    struct GameState *state = gGameState;
    struct GameState *slot = (struct GameState *)(index * 4 + (u32)state);
    if (!slot->counters628[0]) {
        if (++slot->counters620[0] > 35999) {
            ((struct GameState *)((u8 *)state + p->timedEventSlot * 4))->counters620[0] = 0;
            ((struct GameState *)((u8 *)state + p->timedEventSlot * 4))->counters628[0] = 180000;
            Player_CallEventScript(p, 0);
        }
    }
}
