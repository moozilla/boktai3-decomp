#include "actor_attachment.h"

void ActorAttachment_GetAction4Offset(s32 k, s32 *a, s32 *b, s32 d)
{
    switch (k) {
        case 0:
            if (d == 0) {
                *a = -12;
                *b = 0;
            } else {
                *a = 0;
                *b = 0;
            }
            break;
        case 1:
            if (d <= 3) {
                if (d >= 0) {
                    *a = -16;
                    *b = -8;
                }
            }
            break;
        case 2:
            if (d == 0 || d == 3)
                *a = 0;
            else
                *a = -8;
            *b = -4;
            break;
        case 3:
            if (d == 0 || d == 3)
                *a = 0;
            else
                *a = -8;
            *b = -4;
            break;
    }
}
