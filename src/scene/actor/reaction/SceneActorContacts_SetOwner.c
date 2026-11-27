#include "global.h"

extern void *gSceneActorContactManager;
s32 SceneActorContacts_ResetList(void); /* void; its r0 is returned */
/* Makes p the owner of the contact list (gSceneActorContactManager), resets the list and
   stores value at +0x24 (tentative); returns the reset's r0. */
s32 SceneActorContacts_SetOwner(u8 *p, u16 value)
{
    s32 r;
    gSceneActorContactManager = p;
    r = SceneActorContacts_ResetList();
    *(u16 *)(p + 0x24) = value;
    return r;
}
