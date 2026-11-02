#include "global.h"
#include "record_menu.h"
void *ScriptProc_Start(u16, u16);
#include "display_resource.h"
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void RecordMenu_InitializeSprites(struct RecordMenu *menu)
{
    void *proc;
    s32 i;
    struct RecordMenuSprite *sprite;
    proc = ScriptProc_Start(0xcb05, 0x1289);
    menu->rowProc = proc;
    ResourceHeader_Relocate(menu->rowResource, proc);
    i = 0;
    {
        s32 zero = 0;
        sprite = menu->sprites;
        for (; i < 8; i++) {
            Widget_InitLinked(sprite, menu->rowResource, menu->spriteIds[i], 16, zero, zero, zero,
                              (void *)zero);
            menu->states[i] = -1;
            sprite++;
        }
    }
    {
        s32 zero = 0;
        proc = ScriptProc_Start(0xcb05, 0x530d);
        menu->cursorProc = proc;
        ResourceHeader_Relocate(menu->resource, proc);
        Widget_InitLinked(&menu->sprites[8], menu->resource, menu->spriteIds[i], 17, zero, zero,
                          zero, (void *)zero);
    }
    menu->states[i] |= -1;
    CpuSet(menu->sprites[8].tiles, menu->palette, 0x4000008);
    menu->sprites[8].flags = 0xa000;
    menu->sprites[8].tiles = menu->palette;
}
