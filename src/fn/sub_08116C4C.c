#include "global.h"
#include "scene_pool.h"
struct Q {
    u8 f[0x28];
    u32 a, b, c;
};
void Player_GetWorldPosition(s32 i, u32 *d)
{
    struct Q *q;
    if (i <= 1) {
        q = gParticipantActors[i];
        if (q != 0) {
            d[0] = q->a;
            d[1] = q->b;
            d[2] = q->c;
        }
    }
}
