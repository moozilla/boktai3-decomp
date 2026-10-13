#include "global.h"
void sub_0821FF58(void *, int, int, int, int, int);
void SceneActorGraphics_UpdateVariant(u8 *p)
{
    u8 *drawable = p + 0x178;
    {
        u32 mask = 0x40000;
        if (*(u32 *)(p + 0x9c) & mask)
            p[0x3b6] = 1;
        else
            p[0x3b6] = 0;
    }
    if (p[0x3b6] != p[0x3b7]) {
        switch (p[0x3b6]) {
            case 0:
                sub_0821FF58(drawable, *(s16 *)(p + 0x35c), *(s16 *)(p + 0x35e), 0, 0,
                             *(s16 *)(p + 0x360));
                break;
            case 1:
                sub_0821FF58(drawable, *(s16 *)(p + 0x362), *(s16 *)(p + 0x364), 0, 0x60000,
                             *(s16 *)(p + 0x366));
                break;
        }
        {
            int value = p[0x3b6];
            p[0x3b7] = value;
        }
    }
}
