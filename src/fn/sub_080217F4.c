#include "global.h"
void Anim_SetKeyframes(void *, int, u16, u16, u16);
void SceneActors_StartEightDirectionAnimation(u8 *p, u8 *animation, int resource, int sequence,
                                              int speed, int offset, int flags)
{
    int savedFlags = flags;
    flags = savedFlags;
    {
        int direction = p[0xa9];
        if (((u8)(direction - 5)) <= 2) {
            {
                int value = 8 - direction;
                p[0xb6] = value;
            }
            p[0xb7] = 1;
        } else {
            u8 *dst = p + 0xb6;
            int zero = 0;
            *dst = direction;
            p[0xb7] = zero;
        }
    }
    savedFlags = 0;
    {
        int selected = p[0xb6] + sequence;
        *((u16 *)(p + 0xb8)) = selected;
    }
    Anim_SetKeyframes(animation, resource, *((u16 *)(p + 0xb8)), speed, flags);
    flags &= 1;
    if (!flags) {
        p[0xa3] = savedFlags;
    }
    if (p[0xb7]) {
        *((u32 *)(animation + 8)) |= 4;
    } else {
        *((u32 *)(animation + 8)) &= ~4;
    }
}
