#include "global.h"
#include "link/panel/link_panel.h"

void sub_0810C0BC(struct LinkPanel *panel)
{
    s32 i;

    for (i = 0; i < 7; i++) {
        switch (i) {
            case 0:
                panel->animations[i].position.x = i;
                panel->animations[i].position.y = i;
                break;
            case 1:
                panel->animations[i].position.x = 0x90;
                panel->animations[i].position.y = 0;
                break;
            case 6:
                panel->animations[i].position.x = 8;
                panel->animations[i].position.y = 8;
                break;
            default:
                panel->animations[i].position.x = 0;
                panel->animations[i].position.y = 0;
                break;
        }
    }
}
