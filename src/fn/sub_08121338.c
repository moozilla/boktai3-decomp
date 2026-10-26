#include "global.h"

#include "engine.h"
#include "ram.h"
extern u32 gUnk_030053F0;
u8 *RegistryA8CB_FindEntryByName(void *, s32);
void SpatialIndex_RemoveEntry(u8 *);
void SceneMessages_UnregisterReceiver(u8 *);
/* Script: for the A8CB object named -n, removes its five animations
   (Anim_Remove), runs SpatialIndex_RemoveEntry and, unless engine flag 0x800 is set,
   SceneMessages_UnregisterReceiver (tentative). */
void RegistryA8CB_ResetFromScript(void)
{
    void *manager = gActorManager;
    u8 *e;
    if (manager &&
        (e = RegistryA8CB_FindEntryByName(manager, Script_GetKeywordValue('n', 0))) != 0) {
        u8 *p = e;
        u8 *s = e + 0x5F8;
        s32 i;
        for (i = 4; i >= 0; s += 0x60, i--)
            Anim_Remove(s);
        SpatialIndex_RemoveEntry(p + 0x56C);
        {
            u32 mask = 0x800;
            if (!((gEngineFlags | gUnk_030053F0) & mask))
                SceneMessages_UnregisterReceiver(e + 0x858);
        }
    }
}
