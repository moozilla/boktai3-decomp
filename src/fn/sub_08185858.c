#define Armor_UpdateScriptedAttack sub_08185858
#include "global.h"
struct Armor {
    u8 pad0[0x48];
    s16 x,y,z;
    u8 pad4E[0xb3d-0x4e];
    u8 state, timer;
    u8 padB3F;
    u8 animationState;
    u8 padB41[0xb54-0xb41];
    u32 attackScript;
};
struct ArmorScriptArgs { u32 count:16, unused:16; s32 *values; };
s32 sub_0818505C(struct Armor *,s32);
s32 sub_08184FE8(struct Armor *,s32);
void Script_Call(u32,struct ArmorScriptArgs *);
void sub_08185858(struct Armor *p)
{
    s32 values[3];
    struct ArmorScriptArgs args;
    u8 state = p->state;
    switch(state) {
    case 0:
        if(sub_0818505C(p,10)) { p->state=1; p->timer=state; }
        break;
    case 1:
        if(sub_08184FE8(p,12)) {
            u32 script;
            p->state=2; p->timer=0;
            script=p->attackScript;
            if(script) {
                args.count=3; args.values=values;
                values[0]=p->x; values[1]=p->y; values[2]=p->z;
                Script_Call(script,&args);
            }
        }
        break;
    case 2:
        if(p->animationState==255) p->state=255;
        break;
    }
}
