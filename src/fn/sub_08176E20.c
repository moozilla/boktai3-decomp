#include "global.h"
#include "player.h"
#include "scene_hud_display.h"
#ifdef NONMATCHING
// fuzzy: 96.8%
// foundry draft (worker best 95.2%); what differs is mostly register allocation and instruction order
#define SceneHud_CanOpenFromStart sub_08176E20
extern u32 gUnk_030053F4, gUnk_030053F0;
u32 sub_08176E20(struct SceneHudState *p)
{
    struct Player *player;
    if (gUnk_03005260[1] & 8) {
        player=p->actionOwner;
        if (player && player->kind==1 && player->actionMode!=6 && !(player->statusFlags & 1)
            && p->controlMode <= 2) {
            u32 mask=0x201;
            if (!((gUnk_030053F4 | gUnk_030053F0) & mask) && player->historyDirectionMode!=1)
                return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08176E20);
#endif
