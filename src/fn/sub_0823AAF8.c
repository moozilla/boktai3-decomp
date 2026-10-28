#define Player_GetResourceCost LinkPlayer_GetSelectedAbilityCost
#include "global.h"
#include "player.h"
struct ResourceUsage {
    u8 pad0[0x94];
    u32 consumed;
};
extern struct ResourceUsage *gLinkSession;
s32 LinkPlayer_GetSelectedAbilityCost(struct Player *);
void Player_ConsumeActionResource(struct Player *s)
{
    s32 cost = LinkPlayer_GetSelectedAbilityCost(s);
    if (s->currentStatB < cost) {
        if (gLinkSession)
            gLinkSession->consumed += s->currentStatB;
        s->currentStatB = 0;
    } else {
        if (gLinkSession)
            gLinkSession->consumed += cost;
        s->currentStatB -= cost;
    }
}
