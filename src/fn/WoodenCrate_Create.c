#include "global.h"
#include "engine.h"
struct WoodenCrate {
    u8 pad0[0xA4];
    u16 timer;
};
struct WoodenCrate *WoodenCrate_FindByName(s32 name);
u8 WoodenCrate_HasFlags(struct WoodenCrate *, s32 flag);
void sub_080FFAF8(struct WoodenCrate *);
void WoodenCrate_SetFlags(struct WoodenCrate *, s32);
static inline void trigger(struct WoodenCrate *crate, s32 flag)
{
    if (!WoodenCrate_HasFlags(crate, flag)) {
        crate->timer = 24;
        sub_080FFAF8(crate);
        WoodenCrate_SetFlags(crate, 0x10);
    }
}
/* Registry routine 820B (Wooden Crate), given -n: for the crate named -n
   (WoodenCrate_FindByName), -c bit 1 or else bit 2 picks a flag; unless the crate already
   has it, it starts the 24-frame sequence (sub_080FFAF8, state 0x10). */
void WoodenCrate_Create(void)
{
    struct WoodenCrate *crate;
    s32 c;
    s32 name = Script_GetKeywordValue('n', 0);
    if (!name)
        return;
    crate = WoodenCrate_FindByName(name);
    if (!crate)
        return;
    c = Script_GetKeywordValue('c', 0);
    if (c & 1)
        trigger(crate, 1);
    else if (c & 2)
        trigger(crate, 2);
}
