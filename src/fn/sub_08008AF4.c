#include "global.h"
// MATCHING_CONCESSION: register-pinning
// Tentative: pooled breakable/interactable object initialization.
struct Vec {u32 x:16,y:16,z:16;};
struct Slot {u8 pad0[16];u16 id;u8 pad12[34];u8 drawable[96];u8 collider[84];void *resource;};
struct Manager {u8 pad[32];u8 resource[32];};
void sub_08219918(void *,void *,s32,s32,s32,s32,s32,struct Vec *);
void sub_08220D78(void *,void *,s32,s32,s32);
void sub_0821FEB4(void *,u32,u32,u32,u32,struct Vec *,struct Vec *);
void sub_0821FF84(void *,void *,void *);void sub_08008ABC(void);void sub_0821FF24(void *,void *,u32);void sub_0821FE40(void *);
#define Breakable_InitSlot sub_08008AF4
s32 Breakable_InitSlot(struct Manager *manager,struct Slot *slot){
 u32 fill;struct Vec dimensions,scale,offset;void *resource;void *drawable;void *collider;register struct Vec *d asm("r5");
 slot->resource=manager->resource;resource=slot->resource;
 asm("" : "=m"(fill));
 {d=&dimensions;fill=0;{u32 *source=&fill;struct Vec *dest=d;asm("" : : "r"(source),"r"(dest));CpuSet(source,dest,0x05000002);}}
 drawable=slot->drawable;
 sub_08219918(drawable,resource,0,1,2,0,60,d);
 sub_08220D78(drawable,slot->resource,0,1,0);
 collider=slot->collider;
 scale.x=128;scale.y=128;scale.z=128;
 offset.x=0;offset.y=128;offset.z=0;
 sub_0821FEB4(collider,slot->id,0x4001,0,0,&scale,&offset);
 sub_0821FF84(collider,sub_08008ABC,slot);
 sub_0821FF24(collider,(u8 *)slot+0x54,0);
 sub_0821FE40(collider);return 0;
}
