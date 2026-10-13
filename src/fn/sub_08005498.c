#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SceneObjects_Update sub_08005498
struct ObjectTail {u8 damageTimer,mode;u16 animation;u8 pad4[2],active,animationTimer,pad8,cooldown,padA[6];u32 script,id,bodyFlags;u8 pad1C[4];s16 x,y,z;u8 pad26[82];u32 drawFlags;u8 tail[276];};
struct Object {u8 type,flags;u16 state,kind,pad6;u32 owner;u8 padC[10];s16 resource;struct ObjectTail tail;};
struct Manager {u8 task[24];u16 count;u8 frame,descending;s16 fade;u8 fading,stopFade;u32 fromA,fromB,toA,toB;u8 paletteA[32],paletteB[32];struct Object *objects;};
struct Args {u32 count:16;s32 *values;};
extern void (*const gUnk_086049B4[])(struct Object *);
extern u8 *gUnk_02000710;
void sub_0821AD08(u32,struct Args *);
void sub_080046F0(struct Object *);
void sub_080047AC(struct Object *);
void sub_08225B74(void *);
void sub_0821DE30(void *);
void sub_0821FF24(void *,void *,s32);
void sub_08215284(void *,u16);
void sub_08221110(void *,u32,u32,s32,s32);
static inline u8 ownerIsCurrent(u32 owner){if(*(u32 *)(gUnk_02000710+0x5A0)==owner)return 1;return 0;}
static inline void disableCollision(u16 *flags){register u32 value asm("r1")=*flags;{u32 result=value|4;*flags=result;}}
s32 SceneObjects_Update(struct Manager *manager)
{
    struct Object *p=manager->objects;
    s32 i;
    s32 values[5];
    struct Args args;
    for(i=0;i<manager->count;i++,p++){
        if(!p->tail.active)continue;
        if(p->tail.mode){
            if(p->tail.mode==3 && gUnk_086049B4[p->type])gUnk_086049B4[p->type](p);
            if(p->tail.script){
                values[0]=p->tail.id;values[1]=p->type;values[2]=p->tail.x;values[3]=p->tail.y;values[4]=p->tail.z;
                args.count=5;args.values=values;sub_0821AD08(p->tail.script,&args);p->tail.script=0;
            }
            sub_080046F0(p);sub_080047AC(p);continue;
        }
        sub_08225B74((u8 *)p+0x2C);
        if(*(void (**)(struct Object *))((u8 *)p+0x1A4))(*(void (**)(struct Object *))((u8 *)p+0x1A4))(p);
        if(p->flags&0x20){
            if(!p->kind){p->tail.bodyFlags&=~2;sub_0821DE30((u8 *)p+0x174);}
            else p->tail.bodyFlags|=2;
        }else sub_0821DE30((u8 *)p+0x174);
        if(p->tail.cooldown)p->tail.cooldown--;
        if(!p->owner || ownerIsCurrent(p->owner))p->tail.drawFlags&=~1;
        else p->tail.drawFlags|=1;
        if(p->tail.damageTimer || p->resource<=0){p->tail.damageTimer--;goto disable;}
        if(!p->owner || ownerIsCurrent(p->owner)){
            *(u16 *)((u8 *)p+0xD2)&=~4;
            sub_0821FF24((u8 *)p+0xCC,(u8 *)p+0x38,0);
        }else{disable:disableCollision((u16 *)((u8 *)p+0xD2));}
        if(p->tail.animationTimer && !--p->tail.animationTimer)sub_08215284((u8 *)p+0x74,p->tail.animation);
    }
    if(manager->fading){
        sub_08221110(manager->paletteA,manager->fromA,manager->toA,manager->fade,5);
        sub_08221110(manager->paletteB,manager->fromB,manager->toB,manager->fade,5);
        if(!manager->descending){
            if(++manager->fade>31){manager->fade=32;manager->descending=1;}
        }else if(--manager->fade<=0){
            manager->fade=0;manager->descending=0;
            if(manager->stopFade)manager->stopFade=0;
        }
    }
    manager->frame=1-manager->frame;
    return 0;
}
