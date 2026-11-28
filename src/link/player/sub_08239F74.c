#include "global.h"
#include "player/player.h"

s32 sub_08239F74(struct Player *p, s32 mode)
{
    s32 i;

    for (i = 0; i < 2; i++) {
        switch (i) {
            case 0:
                if (p->scriptStatus[0] != 0) {
                    if (p->kind & 1)
                        p->scriptStatus[0]--;
                    mode = 2;
                    p->scriptFlashTimer = 0x40;
                } else if (p->scriptFlashTimer != 0) {
                    if ((p->scriptFlashTimer >> 2) & 1)
                        mode = 2;
                    p->scriptFlashTimer--;
                }
                break;
            case 1:
                if (p->scriptStatus[1] != 0) {
                    if (p->kind & 1) {
                        if (--p->scriptStatus[1] == 0)
                            p->catalogSavedOption = p->actionConfig->catalogOption;
                    }
                }
                break;
        }
    }
    return mode;
}
