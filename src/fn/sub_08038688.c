#include "global.h"
#include "player.h"
struct TradeMenu {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
};
void PlayerCue_ReleaseControl(void *);
void WirelessGameMenu_UpdateStartupDelay(struct TradeMenu *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 1) {
        u32 two = 2;
        u8 *state = &p->state;
        u32 zero = 0;
        *state = two;
        p->entered = 1;
        p->timer = zero;
        PlayerCue_ReleaseControl((*(void **)gPlayers));
    }
    p->timer++;
}
