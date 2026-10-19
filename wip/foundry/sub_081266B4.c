#include "global.h"
#include "player.h"
#define SceneStatusIndicator_UpdateBlink sub_081266B4
#define gSceneStatusSelection            gUnk_020004B4
struct SceneStatusIndicator {
    u32 flags;
    u8 unknown4[0x24];
    u16 state, unknown2A, reset, timer;
};
struct SceneStatusSelection {
    u8 unknown0[0x18];
    s16 selected;
};
extern struct SceneStatusSelection *gUnk_020004B4;
extern u32 gUnk_030053F4;
extern s32 gUnk_02000478;
extern u16 gUnk_02000534, gUnk_02000500;
static inline u8 selectedActive(struct Player *player)
{
    if (player) {
        u32 active = player->active;
        active = active && player->sceneMode == 4;
        return active;
    }
    return 0;
}
static inline u32 flagSet(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u32 worldActive(u32 value)
{
    return value && gUnk_02000500 == 4;
}
struct IndicatorSprite {
    u32 flags;
};
void sub_081266B4(void *unused, struct SceneStatusIndicator *indicator)
{
    struct IndicatorSprite *sprite = (void *)indicator;
    s32 blink = 0;
    if (indicator->reset)
        indicator->reset = 0;
    if (gUnk_030053F4 & 0x800) {
        if (gUnk_020004B4 && gUnk_020004B4->selected >= 0) {
            if (!selectedActive(gUnk_02000580[gUnk_020004B4->selected]))
                goto checked;
            goto blinking;
        }
    } else {
        if (gUnk_02000478 > 0 ||
            (gUnk_02000580[0] && flagSet(&gUnk_02000580[0]->actionFlags, 0x10)))
            blink = 1;
        else {
            u32 active = worldActive(gUnk_02000534);
            if (active)
                blink = 1;
        }
    }
checked:
    if (blink) {
    blinking: {
        u32 state = indicator->state;
        s32 one = 1;
        if (!(one & state)) {
            sprite->flags &= ~1;
            indicator->state |= 1;
        }
        if (indicator->timer) {
            indicator->timer--;
            return;
        }
        {
            u32 flags = indicator->flags;
            if (flags & one)
                flags &= ~1;
            else
                flags |= one;
            indicator->flags = flags;
        }
    }
    } else {
        sprite->flags |= 1;
        indicator->state &= ~1;
    }
    indicator->timer = 38;
}
