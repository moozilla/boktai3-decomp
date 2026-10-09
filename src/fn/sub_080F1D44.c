#include "global.h"
struct Obj { u8 pad0[0xcc4]; s8 red,green,blue; u8 padCC7[0x29]; void (*callback)(struct Obj *); u8 padCF4[8]; s32 counter; u8 padD00[0x32]; u16 color; u8 padD34[0x1e]; s8 duration; u8 padD53,mode; };
s32 Div(s32,s32);
static inline u16 color(struct Obj *p) { return ((u32)p->blue<<10)|((u32)p->green<<5)|(u32)p->red; }
void sub_080F1E00(struct Obj *);
void sub_080F1FBC(struct Obj *);
s32 sub_080F1D44(struct Obj *p)
{
    p->red=Div((u32)p->counter*7,p->duration)+24;
    if(p->counter++==p->duration) {
        if(p->mode==0) p->callback=sub_080F1E00;
        else p->callback=sub_080F1FBC;
        p->counter=p->duration;
    }
    p->color=color(p);
    return 0;
}
