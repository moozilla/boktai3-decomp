#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
extern u16 gUnk_02000534, gUnk_02000500;
u8 *sub_0821D64C(s32, s32);
s32 Scene_TryStepDown(struct SceneObject *input, s32 idx, s32 dir)
{
    struct SceneObject *actor = input;
    register s32 index asm("r4") = idx;
    register s32 direction asm("r8") = dir;
    register struct SceneCell *reference asm("r10") = &actor->cellRef.cell;
    s32 savedIndex;
    s32 delta = 0, dx = 0, dz = 0, speed, step;
    struct SceneCell *cell;
    u8 *dynamic;
    if (({
            u8 active = 0;
            if (gUnk_02000534 && gUnk_02000500 == 2)
                active = 1;
            active;
        }) ||
        actor->type == 1) {
        step = 16;
        speed = 16;
    } else {
        step = 4;
        speed = 64;
    }
    switch (direction) {
        case 0:
            dz = -step;
            delta = -((s16 *)gUnk_030052F4->cells)[2];
            break;
        case 1:
            dx = step;
            delta = 1;
            break;
        case 2:
            dz = step;
            delta = ((s16 *)gUnk_030052F4->cells)[2];
            break;
        case 3:
            dx = -step;
            delta = -1;
            break;
    }
    index += delta;
    dynamic = sub_0821D64C(index, 1);
    savedIndex = index;
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gUnk_030052F4->cells + 12 + savedIndex * 4);
    if (cell) {
        register u32 heightByte asm("r1") = cell->height;
        u32 mask = 0xF0;
        s32 type = heightByte >> 4;
        asm("" : : "r"(heightByte));
        asm("" : "+r"(mask));
        switch (type) {
            case 0: {
                register u32 lowMask asm("r12") = 15;
                u32 referenceHeight;
                register s32 next asm("r3");
                s32 current;
                referenceHeight = ({
                    register struct SceneCell *p asm("r2") = reference;
                    p->height;
                });
                asm("" : : "r"(referenceHeight));
                next = heightByte & lowMask;
                current = referenceHeight & lowMask;
                if ((u32)next >= (u32)current)
                    break;
                if (cell->flags & 8)
                    break;
                if (current - next > 1) {
                    actor->targetY = ({
                        register u32 y asm("r0") = next << 8;
                        y;
                    });
                    actor->direction = ({
                        u32 d = direction;
                        asm("" : "+r"(d));
                        d;
                    });
                    {
                        register void (*callback)(void) asm("r1") = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 2;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 3;
                } else {
                    u32 referenceHeight;
                    actor->targetY = ({
                        register u32 y asm("r0") = next << 8;
                        y;
                    });
                    referenceHeight = ({
                        register struct SceneCell *p asm("r2") = reference;
                        p->height;
                    });
                    {
                        register s32 remaining asm("r0") = referenceHeight & lowMask;
                        if (remaining > 0)
                            remaining--;
                        mask &= referenceHeight;
                        mask |= remaining;
                        reference->height = mask;
                    }
                    {
                        register u32 tile asm("r0") = cell->tile;
                        reference->tile = tile;
                    }
                    actor->direction = ({
                        register u32 d asm("r2") = direction;
                        d;
                    });
                    {
                        register void (*callback)(void) asm("r1") = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 1;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 1;
                }
            }
            default: {
                /* Preserve the original range normalization for non-flat terrain. */
                s32 normalized = type - 1;
                asm("" : : "r"(normalized));
                break;
            }
        }
    }
    return 0;
}
