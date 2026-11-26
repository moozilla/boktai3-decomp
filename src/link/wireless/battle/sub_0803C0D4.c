#include "global.h"
struct P {
    u8 pad[0x10];
    s32 x, y, z;
};
void Player_GetWorldPosition(int, s32 *);
int ScenePool_IsNearPrimaryPlayer(struct P *p)
{
    s32 position[4];
    int distance;
    Player_GetWorldPosition(0, position);
    distance = (position[0] - p->x) >> 12;
    if (distance < 0)
        distance = -distance;
    if (distance <= 100) {
        distance = (position[2] - p->z) >> 12;
        if (distance < 0)
            distance = -distance;
        if (distance <= 30)
            return 1;
    }
    return 0;
}
