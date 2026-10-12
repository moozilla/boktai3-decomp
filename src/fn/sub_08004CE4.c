#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Scene_TryStepDown sub_08004CE4
struct Cell {u8 height,tile;u16 flags;};
struct Object {u8 type,flags;u16 state,kind;u8 pad6[6];s16 dx,dz;u8 direction,moving;u16 targetY,speed;u8 pad16[168];u16 cell;struct Cell terrain;u8 padC4[224];void (*update)(void);};
struct Map {u32 unknown;u8 *cells;};
extern struct Map *gUnk_030052F4;
extern u16 gUnk_02000534,gUnk_02000500;
u8 *sub_0821D64C(s32,s32);
void sub_080050F4(void);
s32 Scene_TryStepDown(struct Object *input,s32 idx,s32 dir)
{
    register struct Object *actor asm("r5")=input;
    register s32 index asm("r4")=idx;
    register s32 direction asm("r8")=dir;
    register struct Cell *reference asm("r10")=&actor->terrain;
    register s32 savedIndex asm("r9");
    s32 delta=0,dx=0,dz=0,speed,step;
    struct Cell *cell;
    u8 *dynamic;
    if(({u8 active=0;if(gUnk_02000534&&gUnk_02000500==2)active=1;active;})||actor->type==1){step=16;speed=16;}
    else{step=4;speed=64;}
    switch(direction){
    case 0:dz=-step;delta=-((s16 *)gUnk_030052F4->cells)[2];break;
    case 1:dx=step;delta=1;break;
    case 2:dz=step;delta=((s16 *)gUnk_030052F4->cells)[2];break;
    case 3:dx=-step;delta=-1;break;
    }
    index+=delta;
    dynamic=sub_0821D64C(index,1);
    savedIndex=index;asm("" : "+r"(savedIndex));
    if(dynamic)cell=(struct Cell *)(dynamic+4);
    else cell=(struct Cell *)(gUnk_030052F4->cells+12+savedIndex*4);
    if(cell){
        register u32 heightByte asm("r1")=cell->height;
        register u32 mask asm("r4")=0xF0;
        s32 type=heightByte>>4;
        asm("" : : "r"(heightByte));
        asm("" : "+r"(mask));
        switch(type){
        case 0:{
            register u32 lowMask asm("r12")=15;
            register u32 referenceHeight asm("r0");
            register s32 next asm("r3");
            s32 current;
            asm("" : : "r"(lowMask));
            referenceHeight=({register struct Cell *p asm("r2")=reference;asm("" : "+r"(p));p->height;});
            asm("" : : "r"(referenceHeight));
            next=heightByte&lowMask;asm("" : : "r"(next));current=referenceHeight&lowMask;
            if((u32)next>=(u32)current)break;
            if(cell->flags&8)break;
            if(current-next>1){
                actor->targetY=({register u32 y asm("r0")=next<<8;asm("" : "+r"(y));y;});actor->direction=({register u32 d asm("r0")=direction;asm("" : "+r"(d));d;});{register void (*callback)(void) asm("r1")=sub_080050F4;asm("" : "+r"(callback));actor->update=callback;}
                actor->state=0;actor->moving=1;actor->dx=dx;actor->dz=dz;actor->kind=2;actor->cell=savedIndex;actor->speed=speed;
                return 3;
            }else{
                u32 referenceHeight;
                actor->targetY=({register u32 y asm("r0")=next<<8;asm("" : "+r"(y));y;});
                referenceHeight=({register struct Cell *p asm("r2")=reference;asm("" : "+r"(p));p->height;});
                {register s32 remaining asm("r0")=referenceHeight&lowMask;asm("" : "+r"(remaining));if(remaining>0)remaining--;
                mask&=referenceHeight;mask|=remaining;reference->height=mask;}
                {register u32 tile asm("r0")=cell->tile;asm("" : "+r"(tile));reference->tile=tile;}
                actor->direction=({register u32 d asm("r2")=direction;asm("" : "+r"(d));d;});{register void (*callback)(void) asm("r1")=sub_080050F4;asm("" : "+r"(callback));actor->update=callback;}
                actor->state=0;actor->moving=1;actor->dx=dx;actor->dz=dz;actor->kind=1;actor->cell=savedIndex;actor->speed=speed;
                return 1;
            }
        }
        default:{
            /* Preserve the original range normalization for non-flat terrain. */
            register s32 normalized asm("r0")=type-1;asm("" : : "r"(normalized));break;
        }
        }
    }
    return 0;
}
