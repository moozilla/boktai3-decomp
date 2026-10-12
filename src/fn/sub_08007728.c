#include "global.h"
struct Vec {s16 x,y,z;};
struct Map {u32 pad0;u8 *cells;u8 pad8[28];u16 rows[1];};
struct Trigger {u8 pad0[8];u8 persistent;u8 pad9[15];u32 timer;u8 pad1C[16];struct Vec *position;u8 pad30[20];void (*update)(void *,struct Trigger *);};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8,gUnk_030052FC;
void *sub_0821D64C(s32,s32);
void sub_08007060(struct Trigger *);void sub_080070D0(void *,struct Trigger *);void sub_080077C4(void *,struct Trigger *);
static inline s32 cellIndex(struct Vec *pos){s32 x=pos->x>>8,z=pos->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
#define Trigger_UpdateGrounded sub_08007728
void Trigger_UpdateGrounded(void *manager,struct Trigger *p){
 if(!p->persistent&&p->timer>899){p->update=sub_080077C4;p->timer=0;}
 else{
 if((p->timer&15)==15){if(sub_0821D64C(cellIndex(p->position),1))p->timer=900;}
 sub_08007060(p);sub_080070D0(manager,p);p->timer++;
 }
}
