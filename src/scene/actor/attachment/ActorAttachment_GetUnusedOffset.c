#include "scene/actor/actor_attachment.h"
/* ActorAttachment_GetUnusedOffset: Unused geometry helper that writes facing-dependent offsets for kind 3 and preserves outputs otherwise */

void ActorAttachment_GetUnusedOffset(s32 kind, s32 *x, s32 *y, s32 dir)
{
    switch (kind) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            switch (dir) {
                case 0:
                    *x = -2;
                    *y = -8;
                    break;
                case 1:
                    *x = -7;
                    *y = -10;
                    break;
                case 2:
                    *x = -7;
                    *y = -7;
                    break;
                case 3:
                    *x = -1;
                    *y = -8;
                    break;
            }
            break;
    }
}
