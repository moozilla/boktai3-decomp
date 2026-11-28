#include "scene/actor/actor_attachment.h"
void ActorAttachment_GetAction25Offset(u32 kind, s32 *x, s32 *y, s32 direction)
{
    switch (kind) {
        case 0:
            *x = -4;
            *y = -15;
            break;
        case 1:
        case 4:
        case 5:
            *x = -16;
            *y = -8;
            break;
        case 2:
            *x = -4;
            *y = -4;
            break;
        case 3:
            switch (direction) {
                case 0:
                    *x = -8;
                    *y = 0;
                    break;
                case 1:
                    *x = 0;
                    *y = 0;
                    break;
                case 2:
                    *x = 0;
                    *y = 2;
                    break;
                case 3:
                    *x = -6;
                    *y = 3;
                    break;
            }
            break;
    }
}
