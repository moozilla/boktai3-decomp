#include "global.h"
#include "game_state.h"
#include "player.h"
#include "scene_hud_display.h"
#define STATUS_WIDGET 79

static inline u32 Offset(u32 base, u32 off)
{
    u32 sum = base + off;
    return sum;
}
static inline void PlaceStatusIcon(struct SceneHudState *p, s32 n,
                                   struct SceneInputPackedPosition pos)
{
    u32 off = (n + STATUS_WIDGET) * sizeof(struct SceneInputWidget);
    *(u32 *)Offset((u32)&p->categoryWidgets[0].flags, off) &= ~1;
    *(struct SceneInputPackedPosition *)Offset((u32)&p->categoryWidgets[0].position, off) = pos;
}
void InventoryPanel_ShowStatusIcons(struct SceneHudState *p)
{
    struct SceneInputPackedPosition pos;
    s32 n = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (((struct GameState *)(p->actionOwner->timedEventSlot * 4 + (u32)gGameState))
            ->counters628[0] > 0) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5e, 1);
        p->statusIcons[n].flags &= ~1;
        *(struct SceneInputPackedPosition *)&p->statusIcons[n].position = pos;
        pos.x = 0xd;
        n = 1;
    }
    if (p->actionOwner->movementModifierActive) {
        if (p->actionOwner->movementModifierKind == 1)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5c, 1);
        else if (p->actionOwner->movementModifierKind == 2)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5b, 1);
        else
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5d, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    } else if (p->actionOwner->scriptStatus[0]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5f, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    }
    if (p->actionOwner->scriptStatus[1]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x60, 1);
        PlaceStatusIcon(p, n, pos);
        n++;
    }
    for (; n <= 3; n++)
        p->statusIcons[n].flags |= 1;
}
