#include "ui/town/shop_lifecycle.h"
s32 SceneInputPanel_AdjustSlidingY(struct AnimSprite *, s32, u16);
void Shop_BeginAlternateConsumableSelection(struct ShopTail *);
s32 Shop_TickDiscardFeedback(struct ShopTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (((struct ShopLifecycle *)p)->frame == 0)
        Sound_PlaySong(270);
    if (SceneInputPanel_AdjustSlidingY(&p->display.buttons[3], 104,
                                       ((struct ShopLifecycle *)p)->frame))
        Shop_BeginAlternateConsumableSelection(p);
    return 0;
}
