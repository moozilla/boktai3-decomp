#include "actor_attachment.h"
void ActorAttachment_GetAction21Offset(s32 kind, s32 *x, s32 *y, s32 direction)
{
    switch (kind) {
        case 0:
            switch (direction) {
                case 0:
                    *x = 220;
                    *y = 32;
                    break;
                case 1:
                    *x = 64;
                    *y = 210;
                    break;
                case 2:
                    *x = -32;
                    *y = 78;
                    break;
                case 3:
                    *x = 64;
                    *y = -32;
                    break;
            }
            break;
        case 1:
            if (direction == 0 || direction == 3) {
                *x = 12;
                *y = -32;
            } else {
                *x = -32;
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
                    *x = 2;
                    *y = 10;
                    break;
                case 1:
                    *x = -8;
                    *y = 14;
                    break;
                case 2:
                    *x = -10;
                    *y = 8;
                    break;
                case 3:
                    *x = 6;
                    *y = 5;
                    break;
            }
            break;
    }
}
