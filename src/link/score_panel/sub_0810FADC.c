#include "global.h"
#include "engine/video/display_resource.h"
#include "ui/save/save_menu.h"

void RecordList_InitializeSprites(struct SaveMenu *menu)
{
    s32 i;
    void *proc;
    proc = ScriptProc_Start(0xcb05, 0x1289);
    menu->widgetScript = proc;
    ResourceHeader_Relocate(&menu->widgetResource, proc);
    for (i = 0; i < 18; i++) {
        AnimSprite_InitLinked(&menu->widgets[i], &menu->widgetResource, menu->glyphs[i], 16, 0, 0,
                              0, 0);
        menu->cachedGlyphs[i] = -1;
    }
}

void sub_0810FADC(struct SaveMenu *menu)
{
    s32 i;
    for (i = 0; i < 18; i++) {
        switch (i) {
            case 1:
                menu->widgets[i].position.x = 0xc8;
                menu->widgets[i].position.y = 0x78;
                break;
            case 2:
                menu->widgets[i].position.x = 0xd0;
                menu->widgets[i].position.y = 0x78;
                break;
            case 3:
                menu->widgets[i].position.x = 0xd8;
                menu->widgets[i].position.y = 0x78;
                break;
            case 4:
                menu->widgets[i].position.x = 0xe0;
                menu->widgets[i].position.y = 0x78;
                break;
            case 5:
                menu->widgets[i].position.x = 0xc8;
                menu->widgets[i].position.y = 0x88;
                break;
            case 6:
                menu->widgets[i].position.x = 0xd0;
                menu->widgets[i].position.y = 0x88;
                break;
            case 7:
                menu->widgets[i].position.x = 0xd8;
                menu->widgets[i].position.y = 0x88;
                break;
            case 8:
                menu->widgets[i].position.x = 0xe0;
                menu->widgets[i].position.y = 0x88;
                break;
            case 9:
                menu->widgets[i].position.x = 0x78;
                menu->widgets[i].position.y = 0x30;
                break;
            case 10:
                menu->widgets[i].position.x = 0x80;
                menu->widgets[i].position.y = 0x30;
                break;
            case 11:
                menu->widgets[i].position.x = 0x88;
                menu->widgets[i].position.y = 0x30;
                break;
            case 12:
                menu->widgets[i].position.x = 0xb8;
                menu->widgets[i].position.y = 0x30;
                break;
            case 13:
                menu->widgets[i].position.x = 0xc0;
                menu->widgets[i].position.y = 0x30;
                break;
            case 14:
                menu->widgets[i].position.x = 0xc8;
                menu->widgets[i].position.y = 0x30;
                break;
            case 15:
                menu->widgets[i].position.x = 0x78;
                menu->widgets[i].position.y = 0x40;
                break;
            case 16:
                menu->widgets[i].position.x = 0x80;
                menu->widgets[i].position.y = 0x40;
                break;
            case 17:
                menu->widgets[i].position.x = 0x88;
                menu->widgets[i].position.y = 0x40;
                break;
            default:
                menu->widgets[i].position.x = 0;
                menu->widgets[i].position.y = 0;
                break;
        }
    }
}
