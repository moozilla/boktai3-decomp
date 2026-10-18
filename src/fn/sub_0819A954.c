#define LinkEffect_UpdateSecondOrb sub_0819A954
#define LinkEffect_UpdateEndpoints sub_0819A698
#define SunOrb_InitParameters      sub_081FE92C
#define SunOrb_CreateAtPosition    sub_081FE760
#include "global.h"
struct LinkVector {
    s16 x, y, z;
};
struct LinkEffect {
    u8 pad0[0x1C];
    u8 *manager;
    struct LinkVector *target;
    struct LinkVector start, end;
    s8 orb, otherOrb;
    s16 collision;
    u16 timer;
    u8 unknown3A, state, emitted;
};

struct SunOrbParameters {
    u8 pad0[4];
    u16 mode;
    u8 unknown6, type;
    u32 flags;
    u8 padC[20];
};
void LinkEffect_UpdateEndpoints(struct LinkEffect *);
void SunOrb_InitParameters(struct SunOrbParameters *);
s32 SunOrb_CreateAtPosition(struct SunOrbParameters *, struct LinkVector *);
void sub_081FEA80(s32);
u32 SunOrb_FadeOutAll(void *);
void SunOrb_SetEffectTimings(void *, s32, u32, u32);
void SunOrb_SetMotionParams(struct SunOrbParameters *, u32, u32, u32, u32);
void sub_0819A954(struct LinkEffect *p)
{
    struct SunOrbParameters params;
    switch (p->state) {
        case 0: {
            s32 index;
            LinkEffect_UpdateEndpoints(p);
            SunOrb_InitParameters(&params);
            params.type = 1;
            params.mode = 2;
            params.flags = 0;
            SunOrb_SetMotionParams(&params, 240, 4, 0, 2);
            SunOrb_SetEffectTimings(&params, p->collision, 15, 30);
            index = SunOrb_CreateAtPosition(&params, &p->end);
            if (index >= 0) {
                sub_081FEA80(index);
                p->otherOrb = index;
            }
            p->state++;
            break;
        }
        case 1:
            if (p->timer > 480) {
                SunOrb_FadeOutAll(p->manager);
                p->state = 255;
            }
            break;
    }
}
