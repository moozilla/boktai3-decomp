#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 47.7%
// Open: the original keeps mode in r4 and ORs it in after `mode == 4`
// instead of the folded constant; mode/two registers swapped.
struct NamedActor {
    u8 pad0[0xB6];
    u16 fieldB6;
    u16 flagsB8;
    u8 padBA[0x26];
    u32 flagsE0;
};
extern void *gUnk_020001AC;
struct NamedActor *sub_08101A3C(s32 name);
void sub_08100A64(struct NamedActor *);
/* Script: for actor -n with flag 2 at +0xE0: +0xB6 = 2, then by -m (default
   1): 4 sets flag 4 and clears 2 at +0xB8; otherwise flag 4 is cleared, flag 2
   set for 2 or cleared, and sub_08100A64 runs (tentative). */
void NamedActor_SetMoveModeFromScript(void)
{
    s32 mode = Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedActor *actor;
    if (gUnk_020001AC && name) {
        actor = sub_08101A3C(name);
        if (actor && (actor->flagsE0 & 2)) {
            s32 two = 2;
            actor->fieldB6 = 2;
            if (mode == 4)
                actor->flagsB8 = (actor->flagsB8 & ~two) | mode;
            else {
                actor->flagsB8 &= ~4;
                if (mode == 2)
                    actor->flagsB8 |= mode;
                else
                    actor->flagsB8 &= ~two;
                sub_08100A64(actor);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", NamedActor_SetMoveModeFromScript);
#endif
