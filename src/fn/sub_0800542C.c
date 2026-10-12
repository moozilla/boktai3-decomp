#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Scene_UpdateObjectFlash sub_0800542C
struct Draw {u8 pad[16];u16 mode;};
struct Object {u8 type,flags;u16 timer,kind;u8 pad6[11];u8 entered;u8 pad12[14];u8 flash;u8 pad21[111];struct Draw draw;u8 padA2[256];void (*update)(void);};
void sub_080050AC(void);
void sub_0822B2F8(s32);
static inline void setMode(struct Draw *d,u16 mode){d->mode=mode;}
void Scene_UpdateObjectFlash(struct Object *p)
{
    if(p->entered){
        p->entered=0;p->flash=3;setMode(&p->draw,3);
        sub_0822B2F8(0x136);
    }
    if(p->flash){
        if(p->timer++>5){p->timer=0;p->flash--;setMode(&p->draw,p->flash);}
    }else{register void (*callback)(void) asm("r0")=sub_080050AC;asm("" : "+r"(callback));p->update=callback;p->timer=0;p->entered=1;p->kind=0;}
}
