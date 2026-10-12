#include "global.h"
#define SceneObjects_TryPushNeighbor sub_08004E8C
struct Vec {s16 x,y,z,pad;};
struct Cell {u8 height,tile;u16 flags;};
struct Object {u8 type,flags;u16 timer,kind;u8 pad6[24];u8 active;u8 pad1F[5];u8 limited,remaining;u8 pad26[152];u16 cell;struct Cell terrain;u8 tail[228];};
struct Manager {u8 pad[24];u16 count;u8 pad1A[86];struct Object *objects;};
struct Map {u32 unused;u8 *cells;u8 pad8[28];u16 rows[1];};
extern struct Manager *gUnk_02000024;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8,gUnk_030052FC;
u8 *sub_0821D64C(s32,s32);
s32 sub_08004CE4(struct Object *,s32,s32);
void sub_080045B0(u8);
static inline s32 cellIndex(struct Vec *pos)
{
    s32 x=pos->x>>8,z=pos->z>>8;
    if(x<0 || z<0 || (u32)x>=gUnk_030052F8 || (u32)z>=gUnk_030052FC)return 0;
    return gUnk_030052F4->rows[z]+x;
}
static inline struct Cell *getCell(s32 index)
{
    u8 *dynamic=sub_0821D64C(index,1);
    struct Cell *cell;
    if(dynamic)cell=(struct Cell *)(dynamic+4);
    else cell=(struct Cell *)(gUnk_030052F4->cells+12+index*4);
    return cell;
}
s32 SceneObjects_TryPushNeighbor(struct Vec *position,s32 direction)
{
    s32 delta,index;
    struct Cell *origin;
    struct Manager *manager=gUnk_02000024;
    struct Object *p;
    s32 i;
    if(!manager)return -1;
    origin=getCell(cellIndex(position));
    index=cellIndex(position);
    delta=0;
    switch(direction){
    case 0:delta=-((s16 *)gUnk_030052F4->cells)[2];break;
    case 1:delta=1;break;
    case 2:delta=((s16 *)gUnk_030052F4->cells)[2];break;
    case 3:delta=-1;break;
    }
    p=manager->objects;
    for(i=0;i<manager->count;p++,i++){
        struct Cell *reference;
        s32 neighbor,result;
        u32 neighborHeight;
        if(!p->active || p->kind)continue;
        reference=&p->terrain;
        neighbor=index+delta;
        if(neighbor!=p->cell)continue;
        if((origin->height&15)+1!=(reference->height&15))continue;
        neighborHeight=getCell(neighbor)->height&15;
        if(neighborHeight!=(reference->height&15))continue;
        if(p->limited && !p->remaining)break;
        result=sub_08004CE4(p,p->cell,direction);
        if(result>0){
            sub_080045B0(p->type);
            if(p->type==1)result=-1;
            if(p->remaining)p->remaining--;
        }
        return result;
    }
    return 0;
}
