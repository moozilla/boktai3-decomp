#include "global.h"
int sub_08220F70(void *, int);
int SceneActors_UpdateAnimation(u8 *p, void *animation, int delta)
{
    p[0xb2] = sub_08220F70(animation, delta);
    {
        int sequence = *(u16 *)(p + 0xb8);
        *(u16 *)(p + 0xba) = sequence;
    }
    if (p[0xb2])
        p[0xa3]++;
    return 0;
}
