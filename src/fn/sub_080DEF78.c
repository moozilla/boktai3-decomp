#include "scene_actor_chase.h"
static inline s32 Direction(s32 x)
{
    return x >= 0 ? x >> 6 : -(-x >> 6);
}
u8 sub_0812E118(s32, s32, s32, s32);
#ifdef NONMATCHING
// fuzzy: 66.8%
// Branch layout, pointer lifetimes and register allocation still differ.
// confidence=low: searches along the facing direction and starts a reachable route.
#define Dog_TryFacingRoute sub_080DEF78
u8 Dog_TryFacingRoute(struct SceneActorRegistryResetView *a, u32 saved)
{
    struct ContactPosition target;
    u8 *context = a->context;
    u8 *group = (u8 *)&a->group;
    s32 tries = 0;
    s32 selected;
    do {
        s32 cell = a->terrainCell;
        s32 height = Chase_HeightAt(cell, &a->position);
        u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
        s32 kind;
        s32 next, count;
        if (data)
            data += 4;
        else
            data = gUnk_030052F4->cells + cell * 4 + 12;
        kind = *data >> 4;
        switch (Direction(a->direction + 32) & 3) {
            case 0:
                next = cell + 1;
                count = 0;
                do {
                    if (!sub_0812E118(cell, next, height, kind))
                        break;
                    next++;
                } while (++count < 1);
                selected = next - 1;
                break;
            case 1:
                next = cell + *(s16 *)(gUnk_030052F4->cells + 4);
                count = 0;
                do {
                    if (!sub_0812E118(cell, next, height, kind))
                        break;
                    next += *(s16 *)(gUnk_030052F4->cells + 4);
                } while (++count < 1);
                selected = next - *(s16 *)(gUnk_030052F4->cells + 4);
                break;
            case 2:
                next = cell - 1;
                count = 0;
                do {
                    if (!sub_0812E118(cell, next, height, kind))
                        break;
                    next--;
                } while (++count < 1);
                selected = next + 1;
                break;
            case 3:
                next = cell - *(s16 *)(gUnk_030052F4->cells + 4);
                count = 0;
                do {
                    if (!sub_0812E118(cell, next, height, kind))
                        break;
                    next -= *(s16 *)(gUnk_030052F4->cells + 4);
                } while (++count < 1);
                selected = next + *(s16 *)(gUnk_030052F4->cells + 4);
                break;
            default:
                selected = a->terrainCell;
        }
        if (selected != a->terrainCell)
            break;
        if (*(s8 *)(context + 0x6A6))
            a->direction += 64;
        else
            a->direction -= 64;
    } while (++tries <= 3);
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    if ((u8)gUnk_0203B400[gUnk_03005308] <= 127)
        context[0x6A6] = 1;
    else
        context[0x6A6] = 0;
    a->direction = saved;
    {
        s32 x = Mod(selected, *(s16 *)(gUnk_030052F4->cells + 4));
        s32 z = Div(selected, *(s16 *)(gUnk_030052F4->cells + 4));
        u8 *data;
        target.x = (x << 8) + 128;
        data = TerrainModifiers_FindCellOverride(selected, 1);
        if (data)
            data += 4;
        else
            data = gUnk_030052F4->cells + selected * 4 + 12;
        target.y = (*data & 15) << 8;
        target.z = (z << 8) + 128;
    }
    if (Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->position, &target)) {
        context[0x6A7]++;
        return 1;
    }
    Callback_InvokeActor(a, *(void **)(group + 0x224));
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DEF78);
#endif
