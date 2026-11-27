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
    struct AnimSpriteResource *resource;
    s32 i;
    position.z = position.y = position.x = 0;
    SceneInputPanel_InitWidgets(&p->display, mode);
    SceneInputPanel_InitHighlight(&p->display, &p->highlight);
    position.x = 64;
    position.y = 88;
    {
        struct SceneInputPackedPosition *location = &position;
        struct AnimSprite *animSprite;
        resource = &p->display.widgetResource;
        animSprite = ((struct ShopPanel *)p)->comparison[0];
        for (i = 0; i < 3; i++) {
            AnimSprite_InitLinked(animSprite, &p->display.widgetResource, 0, 0x30, 0, 0, 60,
                                  location);
            location->x += 8;
            animSprite++;
        }
    }
    position.x = 64;
    position.y = 80;
    {
        struct SceneInputPackedPosition *location = &position;
        struct AnimSprite *animSprite = ((struct ShopPanel *)p)->comparison[1];
        for (i = 0; i < 3; i++) {
            AnimSprite_InitLinked(animSprite, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            animSprite++;
        }
    }
    position.x = 80;
    position.y = 96;
    {
        struct SceneInputPackedPosition *location = &position;
        struct AnimSprite *animSprite = ((struct ShopPanel *)p)->comparison[2];
        for (i = 0; i < 3; i++) {
            AnimSprite_InitLinked(animSprite, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            animSprite++;
        }
    }
    position.x = 80;
    position.y = 104;
    {
        struct SceneInputPackedPosition *location = &position;
        struct AnimSprite *animSprite = ((struct ShopPanel *)p)->comparison[3];
        for (i = 0; i < 3; i++) {
            AnimSprite_InitLinked(animSprite, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            animSprite++;
        }
    }
    position.x = 80;
    position.y = 112;
    {
        struct SceneInputPackedPosition *location = &position;
        struct AnimSprite *animSprite = ((struct ShopPanel *)p)->comparison[4];
        for (i = 0; i < 3; i++) {
            AnimSprite_InitLinked(animSprite, resource, 0, 0x30, 0, 0, 60, location);
            location->x += 8;
            animSprite++;
        }
    }
    SceneInput_InitMarkerPair(&((struct ShopInitTransitions *)p)->transition);
    SceneInput_InitGlyphPair(&((struct ShopInitTransitions *)p)->prompt);
}
