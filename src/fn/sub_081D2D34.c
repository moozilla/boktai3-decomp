#include "global.h"

#include "engine.h"
#include "ram.h"
struct Npc {
    u8 pad0[2];
    u8 field2, field3, mode;
    u8 pad5[3];
    u8 field8;
    u8 pad9[0xB];
    u32 field14;
    u8 pad18[0x203];
    u8 field21B;
};
struct Npc *Actor_FindById(void *, s32);
/* Script: puts the scene NPC named -n into mode 0x1A (sets +0x21B, clears
   the state fields; tentative). */
void SceneNpc_SetSpecialModeFromScript(void)
{
    void *manager = gSceneActorListManager;
    struct Npc *npc;
    if (manager && (npc = Actor_FindById(manager, Script_GetKeywordValue('n', 0))) != 0) {
        u8 mode;
        npc->field21B = 1;
        mode = 0x1A;
        npc->field2 = 0;
        npc->field3 = 0;
        npc->mode = mode;
        npc->field14 = 0;
        npc->field8 = 1;
    }
}
