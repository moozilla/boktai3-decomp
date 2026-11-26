// COMPILER: old_agbcc
#include "global.h"
/* Player_GetAttachmentId: Returns the identifier stored alongside the named-object attachment flags */

u16 Player_GetAttachmentId(u8 *p)
{
    return *(u16 *)(p + 0x476);
}
