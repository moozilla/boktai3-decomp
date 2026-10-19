#include "global.h"
#include "record_menu.h"
#ifdef NONMATCHING
// fuzzy: 90.8%
// foundry draft (worker best 90.8%); what differs is mostly register allocation and instruction order
#define RecordList_InitializeSprites sub_0810FA54
struct RecordList {
    u8 unknown[0x24]; struct RecordMenuSprite sprites[18];
    u8 unknown6e4[0x20]; u8 resource[0x20];
    u16 spriteIds[18]; u16 states[18]; void *spriteProc;
};
void *ScriptProc_Start(u16,u16);
s32 ResourceHeader_Relocate(void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
static inline void SetSpriteProc(void *proc, struct RecordList *list) { list->spriteProc=proc; }
void sub_0810FA54(struct RecordList *list)
{
    s32 i;
    struct RecordMenuSprite *sprite;
    SetSpriteProc(ScriptProc_Start(0xcb05,0x1289),list);
    ResourceHeader_Relocate(list->resource);
    i=0;
    { s32 zero=0;
    sprite=list->sprites;
    for(;i<18;i++) {
        sub_0821983C(sprite,list->resource,list->spriteIds[i],16,zero,zero,zero,(void *)zero);
        list->states[i]=-1;
        sprite++;
    }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810FA54);
#endif
