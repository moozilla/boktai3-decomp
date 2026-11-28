#include "ui/town/shop_lifecycle.h"
s32 SceneInputPanel_AdjustSlidingY(struct AnimSprite *, s32, u16);
void Shop_BeginAlternateSelection(struct ShopTail *);
s32 Shop_TickSaleFeedback(struct ShopTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (((struct ShopLifecycle *)p)->frame == 0)
        Sound_PlaySong(630);
    if (SceneInputPanel_AdjustSlidingY(&p->display.buttons[0], 32,
                                       ((struct ShopLifecycle *)p)->frame))
        Shop_BeginAlternateSelection(p);
    return 0;
}
