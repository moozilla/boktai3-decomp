#include "global.h"
#include "scene_object.h"
u8 *sub_0821D64C(s32, s32);
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline struct SceneCell *getCell(s32 index)
{
    u8 *dynamic = sub_0821D64C(index, 1);
    struct SceneCell *cell;
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gUnk_030052F4->cells + 12 + index * 4);
    return cell;
}
s32 SceneObjects_TryPushNeighbor(struct SceneVec *position, s32 direction)
{
    s32 delta, index;
    struct SceneCell *origin;
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *p;
    s32 i;
    if (!manager)
        return -1;
    origin = getCell(cellIndex(position));
    index = cellIndex(position);
    delta = 0;
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
    p = manager->objects;
    for (i = 0; i < manager->count; p++, i++) {
        struct SceneCell *reference;
        s32 neighbor, result;
        u32 neighborHeight;
        if (!p->active || p->kind)
            continue;
        reference = &p->cellRef.cell;
        neighbor = index + delta;
        if (neighbor != p->cellRef.index)
            continue;
        if ((origin->height & 15) + 1 != (reference->height & 15))
            continue;
        neighborHeight = getCell(neighbor)->height & 15;
        if (neighborHeight != (reference->height & 15))
            continue;
        if (p->pushLimited && !p->pushesLeft)
            break;
        result = Scene_TryStepDown(p, p->cellRef.index, direction);
        if (result > 0) {
            sub_080045B0(p->type);
            if (p->type == 1)
                result = -1;
            if (p->pushesLeft)
                p->pushesLeft--;
        }
        return result;
    }
    return 0;
}
