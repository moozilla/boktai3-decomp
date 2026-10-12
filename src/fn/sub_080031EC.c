#include "palette_controller.h"
// MATCHING_CONCESSION: register-pinning
#define Palette_InitializeFromScript sub_080031EC
extern struct PaletteController *gUnk_02000468;
extern u16 gUnk_03004FC0[];
extern s32 gUnk_03004DA0,gUnk_03004DA8;
extern u16 gUnk_03005214,gUnk_030051C4;
s32 sub_0821ABA8(s32,s32);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
u8 *sub_0821A520(u16,u16);
void sub_08003038(struct PaletteController *);
void sub_0800301C(struct PaletteController *);
void sub_08002F2C(struct PaletteController *);
void sub_08002E54(struct PaletteController *);
void sub_08002F40(struct PaletteController *);
void sub_08002D58(struct PaletteController *,struct PaletteTransition *);
static inline u16 blendEnabled(struct PaletteController *p){return p->flags&8;}
s32 Palette_InitializeFromScript(struct PaletteController *p)
{
    struct PaletteTransition *t=&p->transition;
    struct PaletteWave *w=&p->wave;
    gUnk_02000468=p;
    p->pending=0;
    *(u32 *)((u8 *)p+0x258)=0;
    p->hardware=gUnk_03004FC0;
    gUnk_03004DA0=64;gUnk_03004DA8=64;
    gUnk_03005214=0x1084;gUnk_030051C4=0;
    p->flags=sub_0821ABA8('f',0);
    if(Script_SeekToKeyword('p')) {
        register s32 i asm("r6")=0;u16 **out=p->source;
        while(Script_GetPc() && i<=3) {
            s32 value=Script_GetValue();
            if(value!=0x30)*out=(u16 *)(sub_0821A520(0x92B3,value)+20);else *out=0;
            out++;i++;
        }
    }
    if(Script_SeekToKeyword('d')) {
        register s32 i asm("r6")=0;u16 **out=p->destination;
        while(Script_GetPc() && i<=3) {
            s32 value=Script_GetValue();
            if(value!=0x30)*out=(u16 *)(sub_0821A520(0x92B3,value)+20);else *out=0;
            out++;i++;
        }
    }
    if(p->flags&8)sub_08003038(p);else sub_0800301C(p);
    p->blend=0;
    p->selected=sub_0821ABA8('c',0);
    p->current=p->source[p->selected];
    sub_08002F2C(p);
    if(p->flags&2) {
        p->flags|=8;
        if(Script_SeekToKeyword('a')) {
            w->period=Script_GetValue();w->step=Script_GetValue();w->unused9=Script_GetValue();
            w->unusedC=Script_GetValue();w->lowDelay=Script_GetValue();w->highDelay=Script_GetValue();
        } else {
            w->period=4;w->step=2;w->unused9=8;w->unusedC=0xFFFF;w->lowDelay=32;w->highDelay=0;
        }
        w->timer=0;w->value=0;w->phase=0;
    }
    if(p->flags&1)sub_08002E54(p);
    if(blendEnabled(p) && (p->flags&2))sub_08002F40(p);
    if(p->flags&1)sub_08002D58(p,t);
    if(p->current)CpuSet(p->current,p->hardware,0x04000068);
    p->previousState=gUnk_03005430.state;
    t->mode=0;
    return 0;
}
