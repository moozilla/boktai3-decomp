#include "actor_attachment.h"
#define ActorAttachment_GetActionOffset sub_08064C48
void ActorAttachment_GetActionOffset(s32 kind, u32 action, u32 angle, s32 *x, s32 *y)
{
    angle = (s32)((angle + 0x20) & 0xFF) >> 6;
    switch (action) {
        case 4:
            sub_080648B0(kind, x, y, angle);
            break;
        case 1:
            sub_08064918(kind, x, y, angle);
            break;
        case 3:
            sub_08064954(kind, x, y, angle);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            sub_08064990(kind, x, y, angle);
            break;
        case 21:
            sub_08064A58(kind, x, y, angle);
            break;
        case 25:
            sub_08064B70(kind, x, y, angle);
            break;
        default:
            sub_08064BF8(kind, x, y, angle);
            break;
    }
}
