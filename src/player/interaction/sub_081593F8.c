// COMPILER: old_agbcc
#include "global.h"
/* Player_GetAttachmentId: Returns the identifier stored alongside the named-object attachment flags */
#define Player_GetAttachmentId sub_081593F8

u16 sub_081593F8(u8 *p)
{
    return *(u16 *)(p + 0x476);
}
