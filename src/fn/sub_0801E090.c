#include "global.h"
#define SceneObject_UpdatePoseB sub_0801E090
typedef struct{u32 words[2];} Vec;
extern u32 gUnk_08604CC4[][3];
void SceneObject_UpdatePoseB(u8*p){p[7]=gUnk_08604CC4[p[0x38]][p[0x39]];if(p[0x38]<=1||p[0x38]==4||p[0x38]==5)*(Vec*)(p+0x1c)=*(Vec*)(p+0x70);else{*(u16*)(p+0x1c)=*(u16*)(p+0x70)+40;*(u16*)(p+0x1e)=*(u16*)(p+0x72)+90;*(u16*)(p+0x20)=*(u16*)(p+0x74)+40;}}
