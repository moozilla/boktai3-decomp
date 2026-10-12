#include "global.h"
/* Tentative: test whether a neighboring terrain cell accepts an actor. */
#define Scene_CheckNeighborHeight sub_08004380
struct Cell {u8 height,tile;u16 flags;};
struct Map {u32 unknown;u8 *cells;};
extern struct Map *gUnk_030052F4;
u8 *sub_0821D64C(s32,s32);
void sub_0800435C(void);
s32 Scene_CheckNeighborHeight(u8 *actor,s32 index,s32 direction)
{
    u8 *reference=actor+0xC0;
    s32 delta=0,neighbor;
    struct Cell *cell;
    u8 *dynamic;
    switch(direction){
    case 0:delta=-((s16 *)gUnk_030052F4->cells)[2];break;
    case 1:delta=1;break;
    case 2:delta=((s16 *)gUnk_030052F4->cells)[2];break;
    case 3:delta=-1;break;
    }
    neighbor=index+delta;
    dynamic=sub_0821D64C(neighbor,1);
    if(dynamic)cell=(struct Cell *)(dynamic+4);
    else cell=(struct Cell *)(gUnk_030052F4->cells+12+neighbor*4);
    if(cell){
        if((u32)(cell->height&15)>=(u32)(*reference&15))return 1;
        if(*actor==5)sub_0800435C();
    }
    return 0;
}
