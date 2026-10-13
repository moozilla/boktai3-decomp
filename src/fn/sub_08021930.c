#include "global.h"
typedef struct {
    u8 unknown[12];
    s16 x, y, z;
} Body;
int sub_082215E4(int, int);
int Div(int, int);
void SceneActors_UpdateTargetMetrics(u8 *p)
{
    int x, z;
    Body *body;
    p[0xaa] = 0;
    p[0xe0] = 0;
    body = (Body *)(p + 0x48);
    x = *(s16 *)(p + 0xd0) - body->x;
    z = *(s16 *)(p + 0xd4) - body->z;
    *(int *)(p + 0xdc) = (u8)sub_082215E4(x, z);
    *(int *)(p + 0xd8) = x * x + z * z;
    p[0xe0] = 1;
    if (*(s16 *)(p + 0x96))
        p[0xe1] = Div(*(s16 *)(p + 0x94) << 7, *(s16 *)(p + 0x96));
    else
        p[0xe1] = 0;
    if (p[0xe1] <= 63)
        p[0xa0] = 1;
    else
        p[0xa0] = 0;
}
