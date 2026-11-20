#include "global.h"
#include "record_menu.h"
void *ScriptProc_Start(u16, u16);
#include "display_resource.h"
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void RecordMenu_InitializeSprites(struct RecordMenu *menu)
{
    void *proc;
    s32 i;
    proc = ScriptProc_Start(0xcb05, 0x1289);
    menu->rowProc = proc;
    ResourceHeader_Relocate(menu->rowResource, proc);
    for (i = 0; i < 8; i++) {
        Widget_InitLinked(&menu->sprites[i], menu->rowResource, menu->spriteIds[i], 16, 0, 0, 0,
                          NULL);
        menu->states[i] = -1;
    }
    proc = ScriptProc_Start(0xcb05, 0x530d);
    menu->cursorProc = proc;
    ResourceHeader_Relocate(menu->resource, proc);
    Widget_InitLinked(&menu->sprites[8], menu->resource, menu->spriteIds[i], 17, 0, 0, 0, NULL);
    menu->states[i] |= -1;
    CpuCopy32(menu->sprites[8].tiles, menu->palette, sizeof(menu->palette));
    menu->sprites[8].flags = 0xa000;
    menu->sprites[8].tiles = menu->palette;
}
