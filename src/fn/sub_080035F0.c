#include "palette_controller.h"
// MATCHING_CONCESSION: register-pinning
#define Palette_SetBrightnessFromScript sub_080035F0
extern struct PaletteController *gUnk_02000468;
s32 sub_0821ABA8(s32,s32);
s32 Script_SeekToKeyword(s32);
s32 sub_08002C70(void);
void sub_0821AD08(s32,s32);
void Palette_SetBrightnessFromScript(void)
{
    struct PaletteController *p=gUnk_02000468;
    if(p) {
        struct PaletteTransition *t=({struct PaletteTransition *hint=&p->transition;asm("" : "+r"(hint));hint;});
        s32 enabled=sub_0821ABA8('n',0);
        u32 query=Script_SeekToKeyword('q');
        u32 immediate=((0u-query)|query)>>31;
        s32 callback=sub_0821ABA8('p',0);
        u32 mode;
        *(s32 *)((u8 *)p+0x258)=0;
        if(!(p->flags&16)) {
            p->flags|=16;
            if(t->mode==0){t->brightness=0;t->previousBrightness=0;}
        }
        p->flags|=16;
        if(enabled) {
            if(immediate) {t->mode=2;t->brightness=sub_08002C70();goto complete;}
            if(t->mode==1)goto waiting;
            if(t->mode==2)goto complete;
            mode=1;
            goto setMode;
        } else {
            if(!immediate) {
                if(t->mode==0)goto complete;
                if(t->mode!=3)goto fadeOut;
                goto waiting;
            }
            t->mode=0;t->brightness=0;p->flags&=~16;
        }
complete:
        if(callback)sub_0821AD08(callback,0);
        return;
fadeOut:
        mode=3;
setMode:
        t->mode=mode;
waiting:
        *(s32 *)((u8 *)t+0x20C)=callback;
    }
}
