#include "actor_attachment.h"

void ActorAttachment_GetAction3Offset(s32 k, s32 *a, s32 *b, s32 unused)
{
    switch (k) {
        case 0:
            *a = -4;
            *b = -15;
            break;
        case 1:
            *a = -16;
            *b = -8;
            break;
        case 2:
            *a = -4;
            *b = -4;
            break;
        case 3:
            *a = -4;
            *b = -4;
            break;
    }
}
