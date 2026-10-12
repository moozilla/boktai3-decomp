#include "palette_controller.h"
#define Palette_ApplyFrame sub_0800306C
void sub_08002E80(struct PaletteController *);
void sub_08002EE4(struct PaletteController *);
void sub_08002AA8(struct PaletteController *,u32);
void sub_08002AB0(struct PaletteController *,u32);
u32 sub_08002AB8(struct PaletteController *,u32);
void sub_08002C38(struct PaletteController *,struct PaletteTransition *);
s32 sub_08017444(void);
void sub_0824923C(struct PaletteTransition *,void *);
extern void *gUnk_0860499C[];
void sub_08002F40(struct PaletteController *);
void sub_08002E28(struct PaletteController *,struct PaletteTransition *);
void sub_08002E48(struct PaletteController *,struct PaletteTransition *);
void Palette_ApplyFrame(struct PaletteController *p)
{
    struct PaletteTransition *t=&p->transition;
    if(p->flags&8) {
        if(p->flags&2)sub_08002E80(p);else sub_08002EE4(p);
        if(p->blend!=p->previousBlend)sub_08002AA8(p,7);
    }
    if(p->flags&1) {
        if(gUnk_03005430.state!=p->previousState) {sub_08002C38(p,t);t->level=0;}
        if(t->level<=63)sub_08002AA8(p,6);
    } else sub_08002AB0(p,2);
    if(sub_08017444() || (p->flags&16)) {
        sub_0824923C(t,gUnk_0860499C[t->mode]);
        if(t->brightness!=t->previousBrightness)sub_08002AA8(p,4);
    } else sub_08002AB0(p,4);
    if(sub_08002AB8(p,1))sub_08002F40(p);
    if(sub_08002AB8(p,2))sub_08002E28(p,t);
    if(sub_08002AB8(p,4))sub_08002E48(p,t);
    if(p->current) {
        if(p->flags&32)CpuSet(p->current,p->hardware,0x04000060);
        else CpuSet(p->current,p->hardware,0x04000068);
    }
}
