#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
static inline u32 TestFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
s32 SceneHud_CanOpenSecondarySelection(struct SceneHudState *p)
{
    struct Player *owner = p->actionOwner;
    if (owner->actionMode == 6)
        return 0;
    if (!(gUnk_03005260[0] & 4))
        return 0;
    if (!owner)
        return 0;
    if (owner->kind != 1)
        return 0;
    if (owner->historyDirectionMode == 1)
        return 0;
    if (owner->statusFlags & 1)
        return 0;
    if (!p->secondaryInputEnabled)
        return 0;
    if (p->controlMode > 2)
        return 0;
    if (TestFlags(0x201))
        return 0;
    return 1;
}
