#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 96.8%
// fix.py best version
// foundry draft (worker best 91.4%); what differs is mostly register allocation and instruction order
#define gRandomTable gUnk_0203B400
struct PhaseTarget {
    u8 unknown0[0x30];
    s16 position[3];
};
struct PhaseParent {
    u8 unknown0[0x60];
    struct PhaseTarget *target;
    u8 unknown64[0xD1C];
    u32 flags;
};
struct PhaseActor {
    u8 unknown0[6];
    u8 angle;
    u8 unknown7[0x15];
    s16 position[3];
    u8 unknown22[0x112];
    s16 destination[3];
    u8 unknown13A[2];
    s16 start[3];
    u8 unknown142[2];
    s16 delta[3];
    u8 unknown14A[0x16];
    struct PhaseParent *parent;
    u8 unknown164[0x10];
    void (*update174)(void);
    u8 unknown178[0xC];
    s32 timer;
    u8 unknown188[2];
    s8 direction;
    u8 unknown18B[2];
    s8 angularSpeed;
};
extern const u16 gRandomTable[];
void Sword_UpdateHorizontalWaveFlight(void);
void Sword_UpdateDepthWaveFlight(void);
void ActorPhase_BeginLunge(struct PhaseActor *p)
{
    struct PhaseParent *parent = p->parent;
    s32 speed;
    s32 x;
    s32 z;
    if (!(parent->flags & 3))
        --p->angularSpeed;
    p->angle += (u8)p->angularSpeed;
    speed = p->angularSpeed;
    if (speed < 0)
        speed = -speed;
    if (speed > 9) {
        u32 offset;
        register s32 direction asm("r1");
        u32 random;
        p->start[0] = p->position[0];
        p->start[1] = p->position[1];
        p->start[2] = p->position[2];
        p->destination[0] = parent->target->position[0];
        p->destination[1] = parent->target->position[1] + 220;
        p->destination[2] = parent->target->position[2];
        p->delta[0] = p->destination[0] - p->start[0];
        p->delta[1] = p->destination[1] - p->start[1];
        p->delta[2] = p->destination[2] - p->start[2];
        p->timer = 0;
        gRandomIndex = (u16)((gRandomIndex + 1) & 1023);
        offset = gRandomIndex * 2;
        {
            register u32 tb asm("r2") = (u32)gUnk_0203B400;
            random = *((const u16 *)(offset + tb));
        }
        random &= 1;
        direction = 1;
        if (random) {
            register s32 m asm("r4") = -1;
            asm("" : "+r"(m));
            direction = m;
        }
        p->direction = direction;
        {
            s32 xx = p->delta[0];
            if (xx < 0)
                xx = -xx;
            x = xx;
        }
        {
            s32 zz = p->delta[2];
            if (zz < 0)
                zz = -zz;
            z = zz;
        }
        if (x < z)
            p->update174 = Sword_UpdateHorizontalWaveFlight;
        else
            p->update174 = Sword_UpdateDepthWaveFlight;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", ActorPhase_BeginLunge);
#endif
