#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};struct Shadow{u8 active,flags,pad[6];struct Vec*source;u32 spriteFlags;u8 pad10[20];struct Vec position;};struct Cell{u8 height,tile;u16 flags;};struct Map{u32 pad;u8*cells;u8 pad8[28];u16 rows[1];};extern struct Map*gUnk_030052F4;extern u32 gUnk_030052F8,gUnk_030052FC;u8*sub_0821D64C(int,int);
static inline int cellIndex(struct Vec*p){int x=p->x>>8,z=p->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
static inline struct Cell*cell(int i){u8*p=sub_0821D64C(i,1);if(p)return (void*)(p+4);return (void*)(gUnk_030052F4->cells+12+i*4);}
static inline int noShadow(struct Cell*c){register u32 value asm("r0")=c->flags;asm("" : : "r"(value));value&=32;asm("" : "+r"(value));return value;}
#define Shadow_UpdateTerrain sub_08013458
void Shadow_UpdateTerrain(struct Shadow*p){register u32*flags asm("r6")=&p->spriteFlags;struct Vec*ground=&p->position;struct Vec*source=p->source;int index=cellIndex(source);asm("" : : "r"(flags));if(p->flags&1)ground->y=source->y;else{struct Cell*c=cell(index);int slope=c->height>>4,height=(c->height&15)<<8;switch(slope){case 1:height-=(u8)source->z;break;case 2:height-=(u8)source->x;break;}ground->y=height;}if(source->y<ground->y || ground->y==0 || noShadow(cell(index)))*flags|=1;else{*flags&=~1;ground->x=source->x;ground->z=source->z;}}
