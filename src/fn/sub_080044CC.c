#include "global.h"
#define Scene_PlayNeighborMovementEffect sub_080044CC
struct Vec {s16 x,y,z,pad;} __attribute__((aligned(4)));
u8 sub_08004380(u8 *,s32,s32);
void sub_0800435C(u8);
void sub_08015BD4(s32,s32,s32,s32,s32,s32,s32,s32,s32,s32,struct Vec *);
void Scene_PlayNeighborMovementEffect(u8 *actor)
{
    struct Vec position;
    s32 angle;
    if(sub_08004380(actor,*(u16 *)(actor+0xBE),actor[0x10])){
        position=*(struct Vec *)(actor+0xAC);
        angle=0;
        switch(actor[0x10]){
        case 0:position.z-=128;angle=0xB0;break;
        case 1:position.x+=128;angle=-16;break;
        case 2:position.z+=128;angle=0x30;break;
        case 3:position.x-=128;angle=0x70;break;
        }
        sub_08015BD4(2,1,1,0x40,angle,31,2,4,6,6,&position);
        sub_0800435C(*actor);
    }
}
