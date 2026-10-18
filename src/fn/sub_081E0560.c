#include "global.h"

s32 Anim_Remove(u8 *);

s32 ScriptMessage_Destroy(u8 *p)
{
    return Anim_Remove(p + 0x18);
}
