// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "record_menu.h"
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
s32 ResourceHeader_Relocate(void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void RecordList_InitializeSprites(struct RecordList *list)
{
    s32 i;
    struct RecordMenuSprite *sprite;
    void *proc;
    {
        register void *t asm("r1") = ScriptProc_Start(0xcb05, 0x1289);
        asm("" : "+r"(t));
        proc = t;
    }
    list->spriteProc = proc;
    ResourceHeader_Relocate(list->resource);
    i = 0;
    {
        s32 zero = 0;
        sprite = list->sprites;
        for (; i < 18; i++) {
            Widget_InitLinked(sprite, list->resource, list->spriteIds[i], 16, zero, zero, zero,
                              (void *)zero);
            list->states[i] = -1;
            sprite++;
        }
    }
}
