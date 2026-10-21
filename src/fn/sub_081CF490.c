#include "global.h"

struct SceneNpc {
    u8 unknown0[7];
    u8 phaseComplete, unknown8, phaseEntered;
    u8 unknownA[4];
    u16 flags;
    u32 frame, unknown14;
    u8 phase;
    u8 unknown19[0x2CB];
    void *callback;
};
s32 Callback_InvokeActor(void *, void *);
void SceneNpc_UpdateBlinkPhase(void *unused, struct SceneNpc *npc)
{
    if (npc->phaseEntered) {
        npc->phaseEntered = 0;
        npc->phaseComplete = 0;
        npc->phase = 1;
    }
    if ((u8)Callback_InvokeActor(npc, npc->callback) != 1) {
        if (npc->phase > 16)
            npc->phaseComplete = 1;
        else {
            if (npc->phase & 1) {
                u32 mask = 0x100;
                *(u16 *)((u8 *)npc + 0xE) = mask | *(u16 *)((u8 *)npc + 0xE);
            }
            if (npc->frame > *((u8 *)npc + 0x18)) {
                npc->frame = 0;
                npc->phase = npc->phase + 1;
            }
        }
    }
}
