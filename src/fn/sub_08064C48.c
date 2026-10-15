#include "actor_attachment.h"
void ActorAttachment_GetActionOffset(s32 kind, u32 action, u32 angle, s32 *x, s32 *y, void *unused)
{
    angle = (s32)((angle + 0x20) & 0xFF) >> 6;
    switch (action) {
        case 4:
            ActorAttachment_GetAction4Offset(kind, x, y, angle);
            break;
        case 1:
            ActorAttachment_GetAction1Offset(kind, x, y, angle);
            break;
        case 3:
            ActorAttachment_GetAction3Offset(kind, x, y, angle);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            ActorAttachment_GetActionGroupOffset(kind, x, y, angle);
            break;
        case 21:
            ActorAttachment_GetAction21Offset(kind, x, y, angle);
            break;
        case 25:
            ActorAttachment_GetAction25Offset(kind, x, y, angle);
            break;
        default:
            ActorAttachment_GetDefaultOffset(kind, x, y, angle);
            break;
    }
}
