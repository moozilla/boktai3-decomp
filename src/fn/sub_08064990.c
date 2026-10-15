#include "actor_attachment.h"
void ActorAttachment_GetActionGroupOffset(s32 kind, s32 *x, s32 *y, s32 direction)
{
    switch (kind) {
        case 0:
            switch (direction) {
                case 0:
                    *x = 64;
                    *y = -42;
                    break;
                case 1:
                    *x = -36;
                    *y = 24;
                    break;
                case 2:
                    *x = -86;
                    *y = -12;
                    break;
                case 3:
                    *x = 32;
                    *y = -96;
                    break;
            }
            break;
        case 1:
            if (direction == 0 || direction == 3) {
                *x = 12;
                *y = -32;
            } else {
                *x = -64;
                *y = 0;
            }
            break;
        case 2:
            if (direction == 0 || direction == 3)
                *x = 0;
            else
                *x = -8;
            *y = -4;
            break;
        case 3:
            switch (direction) {
                case 0:
                    *x = 10;
                    *y = 0;
                    break;
                case 1:
                    *x = -14;
                    *y = 2;
                    break;
                case 2:
                    *x = -16;
                    *y = -2;
                    break;
                case 3:
                    *x = 8;
                    *y = -4;
                    break;
            }
            break;
    }
}
