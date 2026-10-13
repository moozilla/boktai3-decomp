#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct Sprite {u32 flags;u8 pad4[3],priority;u8 scaleX,scaleY;u8 padA[6];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Object {u16 id;u8 type,entered,state,flags;u16 timer,parameter;u8 animationDone,effectActive,animated,remove,frameTimer,frame;s16 health;u16 speed,ground,pad16;struct Vec position;void *collider,*attack;u8 resource[28];struct Sprite sprite;struct Anim anim;u8 effect[108];struct Object *previous,*next;};
struct Manager {u8 task[24];void *animation;struct Object *head;};extern struct Manager *gUnk_0200046C;
struct PackedVec {s32 x:16,y:16,z:16,pad:16;};struct Cell {u8 height,tile;u16 flags;};struct Map {u32 pad;u8 *cells;u8 pad8[28];u16 rows[1];};
extern struct Map *gUnk_030052F4;extern u32 gUnk_030052F8,gUnk_030052FC;extern u16 gUnk_08604ABC[];
void *sub_0800D828(u32,u32);void *sub_08219C40(u32);void sub_08219DD8(void *,u32);u8 *sub_0821D64C(s32,s32);
void sub_0821FEB4(void *,u32,u32,u32,u32,void *,void *);void sub_0821FF84(void *,void *,void *);void sub_0821FE40(void *);void sub_0821FF24(void *,void *,u32);void sub_0821FF58(void *,u32,u32,u32,u32,u32);void sub_0800CBAC(void);void sub_0800CBDC(void);
s32 sub_082151E4(void *,u32);void sub_082144A4(void *,void *,u32);void sub_08215284(void *,u32);void sub_08220C8C(void *,void *,u32,u32,u32);
void sub_08013930(void *,void *,u32,u32,u32,u32,u32,u32,u32,u32,u32);s32 sub_0800CBE0(void *,void *);
static inline s32 cellIndex(struct Vec *pos){s32 x=pos->x>>8,z=pos->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
static inline struct Cell *getCell(s32 index){u8 *dynamic=sub_0821D64C(index,1);if(dynamic)return (struct Cell *)(dynamic+4);return (struct Cell *)(gUnk_030052F4->cells+12+index*4);}
#define FallingObject_Create sub_0800CC98
s32 FallingObject_Create(u16 id,s32 type,u32 flags,u32 parameter,u32 speed,u32 health,u32 power,u32 force,struct Vec *position){
struct PackedVec size,offset;struct Object *p;s32 variant;struct Vec *pos;void *resource;
if(!gUnk_0200046C&&!sub_0800D828(0xc0db,0))return -1;
p=sub_08219C40(sizeof(*p));if(!p)return -1;sub_08219DD8(p,sizeof(*p));
p->id=id;variant=0;if(type>1){p->type=2;variant=type-2;}else p->type=type;
p->state=0;p->timer=0;{u32 one=1;p->entered=one;p->flags=flags;p->parameter=parameter;p->position=*position;
if(p->type==0)p->animated=one;else p->animated=0;}p->speed=speed;{struct Vec *groundPos=&p->position;s32 x=p->position.x>>8,z=groundPos->z>>8,index;struct Cell *cell;u32 slope;s32 height;pos=groundPos;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)index=0;else index=gUnk_030052F4->rows[z]+x;cell=getCell(index);slope=cell->height>>4;height=(cell->height&15)<<8;switch(slope){case 1:height-=(u8)groundPos->z;break;case 2:height-=(u8)groundPos->x;break;}p->ground=height;}p->health=health;
if(p->flags&1){
p->collider=sub_08219C40(84);if(!p->collider)return -1;sub_08219DD8(p->collider,84);
if(p->flags&4){size.x=180;size.y=256;size.z=180;offset.x=0;offset.y=256;offset.z=0;}else{size.x=90;size.y=128;size.z=90;offset.x=0;offset.y=128;offset.z=0;}
sub_0821FEB4(p->collider,p->id,0x4001,0,16,&size,&offset);sub_0821FF84(p->collider,sub_0800CBAC,p);sub_0821FE40(p->collider);sub_0821FF24(p->collider,pos,0);
}else p->collider=0;
if((u8)(p->flags&2)){
u32 attackFlags;u32 big;p->attack=sub_08219C40(84);if(!p->attack)return -1;sub_08219DD8(p->attack,84);
big=(u8)(p->flags&4);if(big){big=0;((struct Vec *)&size)->x=120;((struct Vec *)&size)->y=160;((struct Vec *)&size)->z=120;((struct Vec *)&offset)->x=big;((struct Vec *)&offset)->y=80;((struct Vec *)&offset)->z=big;}
else{((struct Vec *)&size)->x=60;((struct Vec *)&size)->y=80;((struct Vec *)&size)->z=60;((struct Vec *)&offset)->x=big;((struct Vec *)&offset)->y=40;((struct Vec *)&offset)->z=big;}
attackFlags=0;if(!p->type)attackFlags=0x200004;
sub_0821FEB4(p->attack,p->id,0x2001,0,32,&size,&offset);sub_0821FF58(p->attack,power,force,65536,attackFlags,60);sub_0821FF84(p->attack,sub_0800CBDC,p);sub_0821FF24(p->attack,pos,0);
}else p->attack=0;
resource=p->resource;if(!sub_082151E4(resource,gUnk_08604ABC[p->type]))return -1;
sub_082144A4(&p->sprite,resource,0);p->sprite.priority=2;p->sprite.position=p->position;
if(type>1)sub_08215284(resource,739+variant);
if(p->flags&4){p->sprite.flags|=2;p->sprite.scaleY=120;p->sprite.scaleX=120;}else{p->sprite.flags&=~2;p->sprite.scaleY=64;p->sprite.scaleX=64;}
if(p->animated)sub_08220C8C(&p->anim,gUnk_0200046C->animation,3,0,0);
sub_08013930(p->effect,pos,80,7,107,107,3200,1,5,1,0);p->effectActive=1;p->remove=0;p->previous=0;p->next=0;return sub_0800CBE0(gUnk_0200046C,p);
}
