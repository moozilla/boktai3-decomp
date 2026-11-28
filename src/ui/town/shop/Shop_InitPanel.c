#include "ui/town/shop_tail.h"
#include "ui/town/shop_panel.h"
struct ShopInitTransitions {
    u8 unknown0[0x1AF0];
    struct SceneInputPair transition;
    struct SceneInputGridView grid;
    struct SceneInputPair prompt;
};
void SceneInputPanel_InitHighlight(struct SceneInputPanel *, struct SceneInputHighlight *);
void Shop_InitPanel(struct ShopTail *p, s32 mode)
{
    struct SceneInputPackedPosition position;
    s32 i;
    position.z = position.y = position.x = 0;
    SceneInputPanel_InitWidgets(&p->display, mode);
    SceneInputPanel_InitHighlight(&p->display, &p->highlight);
    position.x = 64;
    position.y = 88;
    for (i = 0; i < 3; i++) {
        AnimSprite_InitLinked(&((struct ShopPanel *)p)->comparison[0][i],
                              &p->display.widgetResource, 0, 0x30, 0, 0, 60, &position);
        position.x += 8;
    }
    position.x = 64;
    position.y = 80;
    for (i = 0; i < 3; i++) {
        AnimSprite_InitLinked(&((struct ShopPanel *)p)->comparison[1][i],
                              &p->display.widgetResource, 0, 0x30, 0, 0, 60, &position);
        position.x += 8;
    }
    position.x = 80;
    position.y = 96;
    for (i = 0; i < 3; i++) {
        AnimSprite_InitLinked(&((struct ShopPanel *)p)->comparison[2][i],
                              &p->display.widgetResource, 0, 0x30, 0, 0, 60, &position);
        position.x += 8;
    }
    position.x = 80;
    position.y = 104;
    for (i = 0; i < 3; i++) {
        AnimSprite_InitLinked(&((struct ShopPanel *)p)->comparison[3][i],
                              &p->display.widgetResource, 0, 0x30, 0, 0, 60, &position);
        position.x += 8;
    }
    position.x = 80;
    position.y = 112;
    for (i = 0; i < 3; i++) {
        AnimSprite_InitLinked(&((struct ShopPanel *)p)->comparison[4][i],
                              &p->display.widgetResource, 0, 0x30, 0, 0, 60, &position);
        position.x += 8;
    }
    SceneInput_InitMarkerPair(&((struct ShopInitTransitions *)p)->transition);
    SceneInput_InitGlyphPair(&((struct ShopInitTransitions *)p)->prompt);
}
