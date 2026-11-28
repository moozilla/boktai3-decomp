#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"
#include "engine/engine_pause.h"
void Player_GetWorldPosition(u32, s32 *);
static inline s32 absolute(s32 x)
{
    if (x < 0)
        x = -x;
    return x;
}
u8 SpriteDrop_IsNearPlayer(struct SpriteDrop *item)
{
    s32 position[4];
    s32 i;
    if (Engine_PauseBits(0x800)) {
        for (i = 0; i < 2; i++) {
            Player_GetWorldPosition(i, position);
            if (absolute((position[0] >> 12) - item->x) <= 160)
                return 1;
        }
    fail:
        return 0;
    } else {
        Player_GetWorldPosition(0, position);
        if (absolute((position[0] >> 12) - item->x) > 160)
            goto fail;
    }
    return 1;
}
