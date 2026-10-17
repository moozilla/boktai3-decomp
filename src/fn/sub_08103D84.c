#include "global.h"

#define NamedObject_SetPaletteFromScript sub_08103D84
#include "engine.h"
struct Obj {
    u8 pad[0x2C];
    u8 anim[0x1C];
};
extern void *gUnk_020001AC;
struct Obj *sub_08101A3C(s32);
/* Script: palette bank -c + 0x23F for the named object -n. */
void NamedObject_SetPaletteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct Obj *e;
    if (gUnk_020001AC && name && (e = sub_08101A3C(name))) {
        s32 c = Script_GetKeywordValue('c', 0);
        AnimData_SelectPaletteBank(e->anim, c + 0x23F);
    }
}
