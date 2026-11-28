#include "global.h"

#include "scene/actor/modifier_list.h"

u32 ModifierMenu_DestroyDisplayObjects(struct ModifierMenu *menu)
{
    struct ModifierDisplayObject *object = menu->displays;
    s32 i;
    for (i = 0; i < 18; i++, object++)
        Anim_Remove(object);
    return 0;
}
