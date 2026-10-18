#define Player_ConsumeActionResource sub_0823AAF8
#define Player_GetResourceCost LinkPlayer_GetSelectedAbilityCost
#include "global.h"
#include "player.h"
struct ResourceUsage {
    u8 pad0[0x94];
    u32 consumed;
};
extern struct ResourceUsage *gUnk_020004B4;
s32 LinkPlayer_GetSelectedAbilityCost(struct Player *);
void sub_0823AAF8(struct Player *s)
{
    s32 cost = LinkPlayer_GetSelectedAbilityCost(s);
    if (s->currentStatB < cost) {
        if (gUnk_020004B4)
            gUnk_020004B4->consumed += s->currentStatB;
        s->currentStatB = 0;
    } else {
        if (gUnk_020004B4)
            gUnk_020004B4->consumed += cost;
        s->currentStatB -= cost;
    }
}
