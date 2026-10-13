#include "global.h"
#include "scene_object.h"
/* Tentative: test whether a neighboring terrain cell is at least as tall as the object. */
u8 *sub_0821D64C(s32, s32);
u8 Scene_CheckNeighborHeight(struct SceneObject *object, s32 index, s32 direction)
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
        if ((u32)(cell->height & 15) >= (u32)(*reference & 15))
            return 1;
        if (object->type == 5)
            sub_0800435C(object->type);
    }
    return 0;
}
