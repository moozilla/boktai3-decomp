#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Object {u8 pad[32];u32 flags;};
struct Actor {u8 pad[25];s8 status;u8 pad1A[10];struct Object *object;};
#define Actor_CanInteractWithController sub_080096A0
s32 Actor_CanInteractWithController(struct Actor *actor){
 register struct Object *object asm("r1")=actor->object;register u32 mask asm("r0")=0x186c;register u32 flags asm("r1");asm("" : : "r"(object),"r"(mask));flags=object->flags;
 if(flags&mask)return 0;asm("" : "+r"(flags));
 flags&=0x100000;asm("" : : "r"(flags));if(!flags&&actor->status<0)return 1;
 return 0;
}
