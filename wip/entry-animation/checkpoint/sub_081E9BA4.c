#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Hide children outside the current scene, then tick active visible children. */
struct Child {u32 flags;u8 rest[0xa8];void (*update)(void*,void*);u8 index,control;u8 gap[6];s32 count;u32 scenes[8];};
struct Parent {u8 head[0x1c];struct Child children[16];};
extern u8 *gUnk_02000710;
void sub_08249240(void*,void*,void(*)(void*,void*));
static inline u8 equal(u32 a,u32 b){if(a==b)return 1;return 0;}
void sub_081E9BA4(struct Parent*p){s32 i;register s32 *counts asm("r9")=(s32*)((u8*)p+0xd4);register u32 offset asm("r10")=0;u8 *objects=(u8*)p+0x1c;
 for(i=0;i<16;i++,offset+=0xdc){s32 found=0,j;
 for(j=0;j<*(s32*)((u8*)counts+offset);j++){if(equal(*(u32*)(gUnk_02000710+0x5a0),p->children[i].scenes[j]))found=1;}
 if(found)p->children[i].flags&=~1;else p->children[i].flags|=1;
 }
 for(i=0;i<16;i++){u8 *base=(u8*)p+i*0xdc;if((base[0xcd]&4)&&!(*(u32*)(objects+i*0xdc)&1)){if(base[0xeb])--base[0xeb];sub_08249240(p,base+0x1c,*(void(**)(void*,void*))(base+0xc8));}}
}
