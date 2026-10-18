#define Armor_UpdateFacingAction sub_08185E6C
#include "global.h"
struct Armor {
    u8 pad0[0x48];
    s16 x,y,z;
    u8 pad4E[0xb32-0x4e];
    u16 flags;
    u8 padB34[5];
    u8 direction;
    u8 padB3A[3];
    u8 state;
};
extern u8 *gUnk_02000710;
s32 Math_Atan2(s32,s32);
u32 ActorController_StartAlternateTurn(struct Armor *,u32);
s32 sub_08184FE8(struct Armor *,s32);
void sub_08185E6C(struct Armor *p,s32 alternate)
{
    switch(p->state) {
    case 0: {
        u8 *target=gUnk_02000710;
        s32 direction=(Math_Atan2(*(s16 *)(target+0x30)-p->x,*(s16 *)(target+0x34)-p->z)+32)&0xc0;
        if(p->direction!=direction) {
            if(ActorController_StartAlternateTurn(p,direction)) p->state=1;
            break;
        }
        p->state=1;
    }
    case 1:
        if(alternate) {
            if(sub_08184FE8(p,0x20)) p->state=2;
        } else {
            if(sub_08184FE8(p,0x19)) p->state=2;
        }
        break;
    case 2:
        if(p->flags&0x10) p->state=255;
        break;
    }
}
