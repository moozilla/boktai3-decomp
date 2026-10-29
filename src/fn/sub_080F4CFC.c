#include "global.h"
#include "engine.h"
#include "ram.h"

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
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        {
            u32 table = RANDOM_TABLE_ADDR;
            random = *((const u16 *)(offset + table));
        }
        random &= 1;
        p->direction = random ? -1 : 1;
        x = p->delta[0];
        if (x < 0)
            x = -x;
        z = p->delta[2];
        if (z < 0)
            z = -z;
        if (x < z)
            p->update174 = Sword_UpdateHorizontalWaveFlight;
        else
            p->update174 = Sword_UpdateDepthWaveFlight;
    }
}
