// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
s32 ModifierMenu_GetHeaderFrame(struct ModifierMenu *);
void ModifierMenu_UpdateHeaderObjects(struct ModifierMenu *menuArg)
{
    struct ModifierMenu *menu = menuArg;
    register s32 state asm("r5") = menu->state;
    struct ModifierDisplayObject *object;
    switch (state) {
        case 0:
        case 2: {
            struct ModifierGraphicsResource *graphics;
            register u32 mask asm("r8");
            object = &menu->displays[2];
            graphics = &menu->graphicsB;
            Anim_LoadKeyframe(object, graphics, (u16)(menu->mode + 13), 0);
            {
                u32 flags = object->flags;
                mask = ~1;
                object->flags = flags & mask;
            }
            object = &menu->displays[0];
            Anim_LoadKeyframe(object, graphics, 2, 0);
            object->flags &= mask;
            object = &menu->displays[1];
            Anim_LoadKeyframe(object, graphics, (u16)(menu->mode + 3), 0);
            object->flags &= mask;
            object = &menu->displays[3];
            Anim_LoadKeyframe(object, graphics, (u16)(menu->mode + 6), 0);
            object->flags &= mask;
            break;
        }
        case 1: {
            register struct ModifierGraphicsResource *graphics asm("r8");
            register u32 mask asm("r9");
            object = &menu->displays[2];
            graphics = &menu->graphicsB;
            Anim_LoadKeyframe(object, graphics, (u16)(menu->mode + 13), 0);
            {
                u32 flags = object->flags;
                mask = ~1;
                object->flags = flags & mask;
            }
            object = &menu->displays[0];
            object->flags |= state;
            object = &menu->displays[1];
            object->flags |= state;
            object = &menu->displays[3];
            Anim_LoadKeyframe(object, graphics, (u16)ModifierMenu_GetHeaderFrame(menu), 0);
            object->flags &= mask;
            break;
        }
    }
}
