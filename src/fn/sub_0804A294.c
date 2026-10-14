#include "tether_actor.h"
#define TetherActor_SetTransformFromScript sub_0804A294
extern struct TetherActor *gUnk_02000488;
u8 *sub_0821D64C(s32, s32);
void TetherActor_SetTransformFromScript(void)
{
    struct TetherActor *actor = gUnk_02000488;
    if (actor) {
        if (Script_SeekToKeyword('p')) {
            struct ContactPosition *position;
            s32 x, z, cell;
            u8 *properties;
            u32 surface, height;
            actor->savedPosition.x = Script_GetValue();
            actor->savedPosition.y = Script_GetValue();
            actor->savedPosition.z = Script_GetValue();
            position = &actor->savedPosition;
            x = position->x >> 8;
            z = position->z >> 8;
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                cell = 0;
            else {
                u32 row = gUnk_030052F4->rows[z];
                cell = row + x;
            }
            properties = sub_0821D64C(cell, 1);
            if (properties)
                properties += 4;
            else {
                struct EngineMap *map = gUnk_030052F4;
                u32 offset = cell * 4 + 12;
                properties = map->cells + offset;
            }
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)position->z;
                    break;
                case 2:
                    height -= (u8)position->x;
                    break;
            }
            actor->savedPosition.y = height;

            actor->position = actor->savedPosition;
            *(struct ContactPosition *)actor->display.animation.position = actor->savedPosition;
            actor->origin = actor->savedPosition;
        }
        if (Script_SeekToKeyword('d'))
            actor->heading = Script_GetValue();
    }
}
