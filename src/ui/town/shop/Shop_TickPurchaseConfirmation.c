#include "ui/town/shop_tail.h"
void Shop_BeginCatalogTransition(struct ShopTail *);
void Shop_BeginAlternateSelection(struct ShopTail *);
s32 Shop_TickPurchaseConfirmation(struct ShopTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(&p->display, p->highlight.colors);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1)
        Shop_BeginCatalogTransition(p);
    else if (result == 0) {
        Sound_PlaySong(0xDE);
        Shop_BeginAlternateSelection(p);
    }
    return 0;
}
