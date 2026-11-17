// COMPILER: old_agbcc
#include "global.h"

struct SceneActorScriptCursor {
    u8 unknown0[8];
    u16 unknown8;
    u8 unknownA[0x14 - 0xA];
    u16 unknown14;
    u8 unknown16[2];
    u32 unknown18;
};

// confidence=low: clears a script cursor's position fields and the owner's word at +0xDC.
void sub_08136F48(u8 *owner, struct SceneActorScriptCursor *cursor)
{
    cursor->unknown18 = 0u;
    cursor->unknown14 = 0u;
    cursor->unknown8 = 0u;
    *(u32 *)(owner + 0xdc) = 0u;
}
