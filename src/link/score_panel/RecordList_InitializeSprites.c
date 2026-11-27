#include "global.h"
#include "link/lobby/record_menu.h"
struct RecordList {
    u8 unknown[0x24];
    struct RecordMenuSprite sprites[18];
    u8 unknown6e4[0x20];
    u8 resource[0x20];
    u16 spriteIds[18];
    u16 states[18];
    void *spriteProc;
};
void *ScriptProc_Start(u16, u16);
#include "engine/video/display_resource.h"
void AnimSprite_InitLinked(void *, void *, int, int, int, int, int, void *);
void RecordList_InitializeSprites(struct RecordList *list)
{
    s32 i;
    void *proc;
    proc = ScriptProc_Start(0xcb05, 0x1289);
    list->spriteProc = proc;
    ResourceHeader_Relocate(list->resource, proc);
    for (i = 0; i < 18; i++) {
        AnimSprite_InitLinked(&list->sprites[i], list->resource, list->spriteIds[i], 16, 0, 0, 0, 0);
        list->states[i] = -1;
    }
}
void sub_0810FADC(struct RecordList *list)
{
    s32 i = 0;
    do {
        switch (i) {
            case 1:
                list->sprites[i].x = 0xc8;
                list->sprites[i].y = 0x78;
                break;
            case 2:
                list->sprites[i].x = 0xd0;
                list->sprites[i].y = 0x78;
                break;
            case 3:
                list->sprites[i].x = 0xd8;
                list->sprites[i].y = 0x78;
                break;
            case 4:
                list->sprites[i].x = 0xe0;
                list->sprites[i].y = 0x78;
                break;
            case 5:
                list->sprites[i].x = 0xc8;
                list->sprites[i].y = 0x88;
                break;
            case 6:
                list->sprites[i].x = 0xd0;
                list->sprites[i].y = 0x88;
                break;
            case 7:
                list->sprites[i].x = 0xd8;
                list->sprites[i].y = 0x88;
                break;
            case 8:
                list->sprites[i].x = 0xe0;
                list->sprites[i].y = 0x88;
                break;
            case 9:
                list->sprites[i].x = 0x78;
                list->sprites[i].y = 0x30;
                break;
            case 10:
                list->sprites[i].x = 0x80;
                list->sprites[i].y = 0x30;
                break;
            case 11:
                list->sprites[i].x = 0x88;
                list->sprites[i].y = 0x30;
                break;
            case 12:
                list->sprites[i].x = 0xb8;
                list->sprites[i].y = 0x30;
                break;
            case 13:
                list->sprites[i].x = 0xc0;
                list->sprites[i].y = 0x30;
                break;
            case 14:
                list->sprites[i].x = 0xc8;
                list->sprites[i].y = 0x30;
                break;
            case 15:
                list->sprites[i].x = 0x78;
                list->sprites[i].y = 0x40;
                break;
            case 16:
                list->sprites[i].x = 0x80;
                list->sprites[i].y = 0x40;
                break;
            case 17:
                list->sprites[i].x = 0x88;
                list->sprites[i].y = 0x40;
                break;
            default:
                list->sprites[i].x = 0;
                list->sprites[i].y = 0;
                break;
        }
        i++;
    } while (i <= 17);
}
