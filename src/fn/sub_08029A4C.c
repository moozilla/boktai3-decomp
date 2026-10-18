#include "global.h"
void GridAttackActor_GetGridPosition(s16 *p, int x, int z)
{
    if (x != 1 || z) {
        p[0] = (x << 8) + 0xa80;
        {
            int height = 256;
            p[1] = height;
        }
        p[2] = (z << 8) + 0xa80;
    }
}
