#include "global.h"

#include "engine.h"
struct Obj {
    u8 pad[0x2C];
    u8 anim[0x1C];
};
extern void *gUnk_020001AC;
struct Obj *NamedObject_FindByName(s32);
void NamedObject_SetPaletteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct Obj *e;
    if (gUnk_020001AC && name && (e = NamedObject_FindByName(name))) {
        s32 c = Script_GetKeywordValue('c', 0);
        AnimData_SelectPaletteBank(e->anim, c + 0x23F);
    }
}
