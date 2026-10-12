// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActorGraphics_InitializeBody sub_080221DC
typedef struct {s16 x,y,z;} Vec;
void sub_0821FEB4(void*,int,int,int,int,void*,void*);void sub_0821FF24(void*,void*,int);void sub_0821FF84(void*,void*,void*);void sub_0821FF7C(void*,int,int,int);void sub_0821FE40(void*);void sub_080221D8(void);
extern u32 gUnk_030053F4;void sub_0821FF58(void*,int,int,int,int,int);void SceneActorGraphics_InitializeBody(u8*p){Vec scale,rotation;u8*drawable=p+0x178;scale.x=0x40;scale.y=0x80;scale.z=0x40;rotation.x=0;rotation.y=0x80;rotation.z=0;sub_0821FEB4(drawable,0,0x2001,0,16,&scale,&rotation);sub_0821FF24(drawable,p+0x260,0);sub_0821FF84(drawable,sub_080221D8,p);{register int test asm("r0")=gUnk_030053F4&0x1000;int flags;asm("" : "+r"(test));flags=0;if(test)flags=0x40800;sub_0821FF58(drawable,*(s16*)(p+0x35c),*(s16*)(p+0x35e),0,flags,*(s16*)(p+0x360));}}
