// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "player.h"
#include "scene_hud_display.h"
#define STATUS_WIDGET 79
#define STATUS(p, n)  (*(struct SceneInputWidget *)((u8 *)(p) + ((n) * 96 + 0x31C0)))
static inline u32 Offset(u32 base, u32 off)
{
    u32 sum = base + off;
    return sum;
}
#define PLACE0(p, n, pos)                                                                          \
    do {                                                                                           \
        u32 off = ((n) + STATUS_WIDGET) * 96;                                                      \
        *(u32 *)((u32)(p) + 0x1428 + off) &= ~1;                                                   \
        *(struct SceneInputPackedPosition *)((u32)(p) + 0x1440 + off) = pos;                       \
    } while (0)
#define PLACE(p, n, pos)                                                                           \
    do {                                                                                           \
        u32 off = ((n) + STATUS_WIDGET) * 96;                                                      \
        {                                                                                          \
            register u32 *flagAddr asm("r1") = (u32 *)Offset((u32)(p) + 0x1428, off);              \
            *flagAddr &= ~1;                                                                       \
        }                                                                                          \
        *(struct SceneInputPackedPosition *)Offset((u32)(p) + 0x1440, off) = pos;                  \
    } while (0)
void InventoryPanel_ShowStatusIcons(struct SceneHudState *p)
{
    struct SceneInputPackedPosition pos;
    s32 n = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (((struct GameState *)(p->actionOwner->timedEventSlot * 4 + (u32)gGameState))
            ->counters628[0] > 0) {
        Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x5e, 1);
        PLACE0(p, n, pos);
        pos.x = 0xd;
        n = 1;
    }
    if (p->actionOwner->movementModifierActive) {
        if (p->actionOwner->movementModifierKind == 1)
            Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x5c, 1);
        else if (p->actionOwner->movementModifierKind == 2)
            Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x5b, 1);
        else
            Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x5d, 1);
        PLACE(p, n, pos);
        pos.x += 0xd;
        n++;
    } else if (p->actionOwner->scriptStatus[0]) {
        Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x5f, 1);
        PLACE(p, n, pos);
        pos.x += 0xd;
        n++;
    }
    if (p->actionOwner->scriptStatus[1]) {
        Anim_LoadKeyframe(&STATUS(p, n), p->widgetResource, 0x60, 1);
        PLACE(p, n, pos);
        n++;
    }
    for (; n <= 3; n++)
        STATUS(p, n).flags |= 1;
}
