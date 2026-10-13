#include "global.h"
#include "scene_object.h"
/* Tentative: test whether a neighboring terrain cell accepts an object. */
u8 *sub_0821D64C(s32, s32);
u8 Scene_CanEnterNeighborCell(struct SceneObject *object, s32 index, s32 direction)
{
    u8 *reference = &object->cellRef.cell.height;
    s32 delta = 0, neighbor;
    struct SceneCell *cell;
    u8 *dynamic;
    switch (direction) {
        case 0:
            delta = -((s16 *)gUnk_030052F4->cells)[2];
            break;
        case 1:
            delta = 1;
            break;
        case 2:
            delta = ((s16 *)gUnk_030052F4->cells)[2];
            break;
        case 3:
            delta = -1;
            break;
    }
    neighbor = index + delta;
    dynamic = sub_0821D64C(neighbor, 1);
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gUnk_030052F4->cells + 12 + neighbor * 4);
    if (cell) {
        if (!(cell->height >> 4)) {
            if ((u32)(cell->height & 15) < (u32)(*reference & 15))
                goto fail;
            if (cell->flags & 8)
                goto fail;
            goto success;
        } else if ((u32)(cell->height & 15) >= (u32)(*reference & 15))
            goto success;
    }
fail:
    return 0;
success:
    return 1;
}
