#include "area_effect_link.h"
#include "engine.h"
#include "ram.h"
struct StatusSlot {
    u32 flags;
    u8 pad4[0x24];
    u16 state;
    u8 pad2A[2];
    u16 reset, timer;
};
struct StatusPlayer {
    u8 unknown0[0x448];
    u32 flags;
    u8 unknown44C[0x534 - 0x44C];
    u16 interactionMode;
    u8 unknown536[2];
    u16 passageState;
};
static inline u8 PlayerInMode(struct StatusPlayer *p)
{
    if (p) {
        u32 value = p->passageState;
        u32 active = 0;
        if (value && p->interactionMode == 4)
            active = 1;
        return active;
    }
    return 0;
}
static inline u8 GlobalMode(void)
{
    u32 value = gPlayerPassageState;
    u32 active = 0;
    if (value && gPlayerInteractionMode == 4)
        active = 1;
    return active;
}
static inline u32 TestMask(u32 *p, u32 mask)
{
    return *p & mask;
}
// confidence=low: blinks an indicator for the selected player's active mode.
#define SceneStatusIndicator_UpdateBlink sub_081266B4
void SceneStatusIndicator_UpdateBlink(void *owner, struct StatusSlot *input)
{
    struct StatusSlot *p = input;
    struct StatusSlot *sprite = p;
    u32 active = 0;
    if (p->reset)
        p->reset = active;
    if (gEngineFlags & 0x800) {
        s32 local;
        if (gLinkSession && (local = gLinkSession->localPlayer) >= 0) {
            if (!PlayerInMode((struct StatusPlayer *)gPlayers[local]))
                goto checkActive;
            goto show;
        }
    } else {
        if (gWeatherCollisionTimer > 0 ||
            (gPlayers[0] && TestMask(&((struct StatusPlayer *)gPlayers[0])->flags, 16)) ||
            GlobalMode())
            active = 1;
    }
checkActive:
    if (active) {
    show:
        if (!(p->state & 1)) {
            sprite->flags &= ~1;
            p->state |= 1;
        }
        {
            u32 timer = p->timer;
            if (timer) {
                p->timer = timer - 1;
                return;
            }
        }
        {
            u32 flags = p->flags;
            if (flags & 1)
                flags &= ~1;
            else
                flags |= 1;
            p->flags = flags;
        }
    } else {
        sprite->flags |= 1;
        p->state &= 0xFFFE;
    }
    p->timer = 38;
}
