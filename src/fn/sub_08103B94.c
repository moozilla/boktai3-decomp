#include "global.h"
#include "engine.h"
#include "ram.h"
#ifdef NONMATCHING
// fuzzy: 85.9%
// fix.py best version (reprinted C)
// Open: the original keeps mode in r4 and ORs it in after `mode == 4`
// instead of the folded constant; mode/two registers swapped.
struct NamedActor {
    u8 pad0[0xB6];
    u16 fieldB6;
    u16 flagsB8;
    u8 padBA[0x26];
    u32 flagsE0;
};
struct NamedActor *NamedObject_FindByName(s32 name);
void NamedActor_PlayMovementSound(struct NamedActor *);
/* Script: for actor -n with flag 2 at +0xE0: +0xB6 = 2, then by -m (default
   1): 4 sets flag 4 and clears 2 at +0xB8; otherwise flag 4 is cleared, flag 2
   set for 2 or cleared, and NamedActor_PlayMovementSound runs (tentative). */
void NamedActor_SetMoveModeFromScript(void)
{
    s16 mode = (u8)Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedActor *actor;
    u32 k1 = 4;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor && (actor->flagsE0 & 2)) {
            s32 two = 2;
            actor->fieldB6 = 2;
            if (mode == k1)
                actor->flagsB8 = (actor->flagsB8 & (~two)) | mode;
            else {
                actor->flagsB8 &= ~k1;
                if (mode == 2)
                    actor->flagsB8 |= mode;
                else
                    actor->flagsB8 &= ~two;
                NamedActor_PlayMovementSound(actor);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", NamedActor_SetMoveModeFromScript);
#endif
