#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
#include "engine_pause.h"
s32 SceneHud_CanOpenSecondarySelection(struct SceneHudState *p)
{
    struct Player *owner = p->actionOwner;
    if (owner->actionMode == 6)
        return 0;
    if (!(gPadState[0] & 4))
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
    if (Engine_PauseBits(0x201))
        return 0;
    return 1;
}
