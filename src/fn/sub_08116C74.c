#include "global.h"
#include "area_effect_link.h"
#include "engine.h"
#include "ram.h"
#define gLinkSelection gUnk_020004B4
struct LinkParticipant {
    u8 unknown0[0x8F8];
    s8 playerIndex;
};
static inline s32 LinkSelection_GetLocalPlayer(void)
{
    struct AreaEffectLinkState *selection = gUnk_020004B4;
    if (!selection)
        return -1;
    return selection->localPlayer;
}
u8 LinkParticipant_IsLocallySelected(struct LinkParticipant *participant)
{
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        s32 index = participant->playerIndex;
        s32 selected = LinkSelection_GetLocalPlayer();
        if (index != selected)
            return 0;
    }
    return 1;
}
