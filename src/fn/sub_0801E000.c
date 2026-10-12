#include "global.h"
#define SceneObject_UpdatePoseA sub_0801E000
typedef struct{u32 words[2];} Vec;
extern u32 gUnk_08604C94[][3];
void SceneObject_UpdatePoseA(u8*p){p[7]=gUnk_08604C94[p[0x38]][p[0x39]];if(p[0x39]==2){*(u16*)(p+0x1c)=*(u16*)(p+0x70)-40;*(u16*)(p+0x1e)=*(u16*)(p+0x72)-90;*(u16*)(p+0x20)=*(u16*)(p+0x74)-40;}else if(p[0x39]==0&&(u8)(p[0x38]-2)<=1){*(u16*)(p+0x1c)=*(u16*)(p+0x70)+80;*(u16*)(p+0x1e)=*(u16*)(p+0x72)+180;*(u16*)(p+0x20)=*(u16*)(p+0x74)+80;}else *(Vec*)(p+0x1c)=*(Vec*)(p+0x70);}
