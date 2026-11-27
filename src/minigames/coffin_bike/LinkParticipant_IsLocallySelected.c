#include "global.h"
#include "engine/engine_pause.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine.h"
#include "engine/ram.h"
struct LinkParticipant {
    u8 unknown0[0x8F8];
    s8 playerIndex;
};
static inline s32 LinkSelection_GetLocalPlayer(void)
{
    struct AreaEffectLinkState *selection = gLinkSession;
    if (!selection)
        return -1;
    return selection->localPlayer;
}
u8 LinkParticipant_IsLocallySelected(struct LinkParticipant *participant)
{
    if (Engine_PauseBits(0x800)) {
        s32 index = participant->playerIndex;
        s32 selected = LinkSelection_GetLocalPlayer();
        if (index != selected)
            return 0;
    }
    return 1;
}
