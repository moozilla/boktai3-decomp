#include "global.h"
struct TerrainModifier {u16 id;u8 active,kind,tile,height;u16 flags,cell;u8 padA[2];u8 registration[16];struct TerrainModifier *previous,*next;};
struct Manager {u8 pad[24];struct TerrainModifier *head;};
struct Vec {s16 x,y,z,pad;};
struct Map {u8 pad[36];u16 rows[1];};
extern struct Manager *gUnk_02000028;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8,gUnk_030052FC;
static inline s32 heightLevel(s32 value){if(value>=0)return value>>8;return -((-value)>>8);}
static inline s32 cellIndex(struct Vec *pos){s32 x=pos->x>>8,z=pos->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
#define TerrainModifiers_Create sub_08005E64
void *sub_08006144(s32,s32);
void *sub_08219C40(u32);
void sub_08219DD8(void *,u32);
void sub_0821D698(void *,s32,s32,s32,s32,s32);
void sub_08005E1C(struct TerrainModifier *);
s32 TerrainModifiers_Create(s32 id,struct Vec *position,s32 kind,s32 tile,s32 flags)
{
    struct TerrainModifier *p;
    if(!gUnk_02000028 && !sub_08006144(0x2153,0))return 0;
    p=sub_08219C40(36);
    if(!p)return -1;
    sub_08219DD8(p,36);
    p->id=id;p->active=1;p->kind=kind;p->tile=tile;
    p->cell=cellIndex(position);p->height=heightLevel(position->y);p->flags=flags;
    sub_0821D698(p->registration,p->cell,p->tile,p->height,255,p->flags);
    sub_08005E1C(p);
    return 0;
}
