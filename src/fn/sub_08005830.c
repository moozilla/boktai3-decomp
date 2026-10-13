#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SceneObjects_CreateObject sub_08005830
struct Vec {s16 x,y,z,pad;};
struct Size {u32 x:16,y:16,z:16;};
struct Body {u32 id,unknown,flags;struct Vec position;u8 tail[52];};
struct Animation {u8 pad[6];u16 initial;u8 tail[20];};
struct Draw {u8 pad[16];u16 mode;u8 tail[10];};
struct Collider {u8 pad[52];u32 flags,flags2;u8 tail[24];};
struct Link {u16 id;u8 pad2[2];struct Vec *position;u8 tail[40];};
struct Cell {u8 height,tile;u16 flags;};
struct Object {u8 type,flags;u16 timer,kind,unknown6;u32 owner;u16 dx,dz;u8 direction,entered;u16 targetY,duration; s16 resource;u8 damageTimer,mode;u16 initialAnimation,animation;u8 active,animationTimer,phase,cooldown,randomPhase,unk23,typeFiveA,typeFiveB,pad26[2];u32 script;struct Body body;struct Animation animationData;struct Draw draw;struct Vec center;u8 padB4[8];u16 cellId,cellIndex;struct Cell cell;u8 padC4[8];struct Collider collider,stepCollider;struct Link link;void (*update)(void);};
struct Manager {u8 pad[30];u8 fading;};
struct Map {u32 unused;u8 *cells;u8 pad8[28];u16 rows[1];};
extern struct Manager *gUnk_02000024;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8,gUnk_030052FC,gUnk_03005308;
extern u16 gUnk_0203B400[];
struct Object *sub_080057D0(struct Manager *);
void sub_08219DD8(void *,u32);
void sub_082151E4(struct Animation *,u32);
void sub_082144A4(struct Draw *,struct Animation *,s32);
void sub_08225AB4(struct Body *,u32,struct Vec *,s32,s32,struct Object *);
void sub_08225B28(struct Body *,struct Draw *);
u32 sub_0822C034(struct Vec *);
void sub_0800542C(void),sub_080050AC(void);
u8 *sub_0821D64C(s32,s32);
u32 sub_0821D698(void *,u32,u32,u32,u32,u32);
void sub_0821FEB4(struct Collider *,u16,u32,s32,s32,struct Size *,struct Size *);
void sub_0821FF24(struct Collider *,struct Vec *,s32);
void sub_0821FF84(struct Collider *,void (*)(void),struct Object *);
void sub_0821FE40(struct Collider *);
void sub_0821FF50(struct Collider *,s32,s32);
void sub_080045E8(void),sub_080046EC(void);
s32 Mod(s32,s32);
void sub_08215284(struct Animation *,u16);
s32 sub_08003C00(struct Body *);
static inline void initializeCell(void *cell,s32 index,s32 height){sub_0821D698(cell,index,0,height,255,({register s32 zero asm("r2")=0;zero;}));}
static inline void setStepFlags(struct Collider *c){c->flags=0x800;c->flags2=0;}
static inline u16 flags16(struct Object *p,u32 mask){return mask&p->flags;}
s32 SceneObjects_CreateObject(u32 id,s32 type,u32 flags,struct Vec *position,u32 animationResource,s32 drawMode,s32 resource,s32 initialAnimation,s32 animation,u32 script)
{
    struct Size size,offset;
    struct Manager *manager=gUnk_02000024;
    struct Object *p;
    register struct Draw *draw asm("r5");
    register struct Vec *pos asm("r9");
    s32 x,z,index,height;
    u32 clear;
    u8 *dynamic;
    struct Cell *cell;
    if(!manager || !(p=sub_080057D0(manager)))return -1;
    sub_08219DD8(p,0x1A8);
    p->active=1;p->type=type;
    if(p->type==6)manager->fading=1;
    p->flags=flags;p->timer=0;p->kind=0;
    sub_082151E4(&p->animationData,animationResource);
    draw=&p->draw;asm("" : "+r"(draw));
    sub_082144A4(draw,&p->animationData,0);
    draw->mode=drawMode;
    p->center.x=(position->x&~255)+128;p->center.y=position->y&~255;p->center.z=(position->z&~255)+128;
    sub_08225AB4(&p->body,id,position,0,7,p);
    sub_08225B28(&p->body,draw);
    p->body.position=p->center;
    if(flags16(p,16)){p->owner=sub_0822C034(&p->body.position);pos=&p->body.position;}else{p->owner=0;pos=&p->body.position;}
    p->dx=0;p->dz=0;p->direction=0;
    if(flags16(p,4)){{register void (*callback)(void) asm("r0")=sub_0800542C;asm("" : "+r"(callback));p->update=callback;}p->timer=0;}else{{register void (*callback)(void) asm("r0")=sub_080050AC;asm("" : "+r"(callback));p->update=callback;}p->timer=0;}
    p->entered=1;
    x=p->body.position.x>>8;z=({register struct Vec *coords asm("r1")=pos;asm("" : "+r"(coords));coords->z;})>>8;
    if(x<0 || z<0 || (u32)x>=gUnk_030052F8 || (u32)z>=gUnk_030052FC)index=0;
    else index=gUnk_030052F4->rows[z]+x;
    dynamic=sub_0821D64C(index,1);
    if(dynamic)cell=(struct Cell *)(dynamic+4);else cell=(struct Cell *)(gUnk_030052F4->cells+12+index*4);
    height=cell->height&15;if(height<15)height++;
    initializeCell(&p->cellId,index,height);
    {register u32 y asm("r0")=height<<8;asm("" : "+r"(y));
    clear=({register u32 zero asm("r3")=0;asm("" : "+r"(zero));zero;});
    p->targetY=y;}
    size.x=130;size.y=128;size.z=130;offset.x=0;offset.y=128;offset.z=0;
    sub_0821FEB4(&p->collider,p->body.id,0x4041,0,16,&size,&offset);
    sub_0821FF24(&p->collider,pos,0);
    sub_0821FF84(&p->collider,sub_080045E8,p);
    sub_0821FE40(&p->collider);
    ((u16 *)&size)[0]=128;((u16 *)&size)[1]=128;((u16 *)&size)[2]=128;
    sub_0821FEB4(&p->stepCollider,p->body.id,0x2801,0,1,&size,&offset);
    sub_0821FF24(&p->stepCollider,pos,0);
    sub_0821FF84(&p->stepCollider,sub_080046EC,p);
    sub_0821FF50(&p->stepCollider,1500,50);
    setStepFlags(&p->stepCollider);
    p->resource=resource;p->damageTimer=clear;p->mode=clear;p->animationTimer=clear;p->phase=clear;p->cooldown=clear;p->unk23=4;
    gUnk_03005308=(gUnk_03005308+1)&1023;p->randomPhase=Mod(({u32 offset=gUnk_03005308*2;register u16 *table asm("r2");asm("" : "+r"(offset));table=gUnk_0203B400;asm("" : "+r"(table));*(u16 *)(offset+(u32)table);}),16);
    {register s32 initial asm("r3")=initialAnimation;asm("" : "+r"(initial));
    if(initial>=0)p->initialAnimation=initial;else p->initialAnimation=p->animationData.initial;}
    sub_08215284(&p->animationData,p->initialAnimation);
    p->animation=animation;p->link.id=p->body.id;p->link.position=pos;p->script=script;
    if(!(p->flags&2) && sub_08003C00(&p->body)<0)p->flags|=2;
    if(p->type==5){p->typeFiveA=1;p->typeFiveB=1;}else{p->typeFiveA=0;p->typeFiveB=0;}
    return 0;
}
