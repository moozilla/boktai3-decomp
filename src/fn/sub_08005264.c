#include "global.h"
#define Scene_UpdateFallingObject sub_08005264
struct Body {u8 pad[14];s16 y;u8 pad10[6];s16 vy;};
struct Cell {u8 height,tile;u16 flags;};
struct Object {u8 type,flags;u16 state,kind;u8 pad6[6];u16 dx,dz;u8 direction,entered;u16 targetY;u8 pad14[2];u16 resource;u8 pad18[20];struct Body body;u8 pad44[124];struct Cell terrain;u8 padC4[224];void (*update)(void);};
void sub_080050AC(void);
void sub_08004450(struct Object *);
void Scene_UpdateFallingObject(struct Object *p)
{
    struct Body *body=&p->body;
    struct Cell *terrain=&p->terrain;
    if(p->entered){
        s32 height;
        p->entered=0;
        height=(p->targetY>>8)+1;
        if(height>15)terrain->height=(terrain->height&0xF0)|15;
        else terrain->height=(terrain->height&0xF0)|height;
        if(({u32 mask=8;mask&p->flags;}))p->terrain.flags|=0x21;
    }
    body->vy=p->targetY-body->y;
    if(body->vy< -16)body->vy=-16;
    else if(body->vy>0)body->vy=0;
    if(body->vy==0){
        if(({u32 mask=8;mask&p->flags;})){p->terrain.flags|=0x21;p->resource=0;}
        else{p->terrain.flags&=~0x21;sub_08004450(p);}
        {register void (*callback)(void) asm("r1")=sub_080050AC;p->update=callback;}p->state=0;p->entered=1;p->dx=0;p->dz=0;p->direction=0;p->kind=0;
    }
}
