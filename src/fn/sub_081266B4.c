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
extern u16 gUnk_02000534;
static inline u8 PlayerInMode(struct AreaEffectPlayer *p)
{
    if (p) {
        u32 value = *(u16 *)((u8 *)p + 0x538);
        u32 active = 0;
        if (value && *(u16 *)((u8 *)p + 0x534) == 4)
            active = 1;
        return active;
    }
    return 0;
}
static inline u8 GlobalMode(void)
{
    u32 value = gUnk_02000534;
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
        if (gUnk_020004B4 && (local = gUnk_020004B4->localPlayer) >= 0) {
            if (!PlayerInMode(gUnk_02000580[local]))
                goto checkActive;
            goto show;
        }
    } else {
        if (gWeatherCollisionTimer > 0 ||
            (gUnk_02000580[0] && TestMask((u32 *)((u8 *)gUnk_02000580[0] + 0x448), 16)) ||
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
