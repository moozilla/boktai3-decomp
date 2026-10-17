#include "global.h"

extern void *gUnk_020001A0;
s32 SceneActorContacts_ResetList(void); /* void; its r0 is returned */
/* Makes p the owner of the contact list (gUnk_020001A0), resets the list and
   stores value at +0x24 (tentative); returns the reset's r0. */
s32 SceneActorContacts_SetOwner(u8 *p, u16 value)
{
    s32 r;
    gUnk_020001A0 = p;
    r = SceneActorContacts_ResetList();
    *(u16 *)(p + 0x24) = value;
    return r;
}
