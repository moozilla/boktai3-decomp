#include "global.h"
struct PhaseParent {
    u8 unknown0[0xD80];
    u32 flags;
};
struct PhaseActor {
    u8 unknown0[6];
    u8 angle;
    u8 unknown7[0x15];
    s16 position[3];
    u8 unknown22[0x30];
    u16 field52;
    u8 unknown54[0x52];
    u16 fieldA6;
    u8 unknownA8[0x58];
    s16 velocity[3];
    u8 unknown106[0x2E];
    s16 destination[3];
    u8 unknown13A[2];
    s16 start[3];
    u8 unknown142[2];
    s16 delta[3];
    u8 unknown14A[0x16];
    struct PhaseParent *parent;
    u8 unknown164[0x14];
    void (*update178)(void);
};
void Sword_UpdateThrustRetraction(void);
void ActorPhase_DampMovement(struct PhaseActor *p)
{
    struct PhaseParent *parent = p->parent;
    p->angle += 20;
    p->position[0] += p->velocity[0];
    p->position[2] += p->velocity[2];
    if (!(parent->flags & 7)) {
        s32 value, half;
        if (p->velocity[0] == 0 && p->velocity[2] == 0) {
            p->fieldA6 = 16;
            p->field52 = 16;
            p->destination[0] = p->position[0];
            p->destination[1] = p->position[1];
            p->destination[2] = p->position[2];
            p->delta[0] = p->destination[0] - p->start[0];
            p->delta[1] = p->destination[1] - p->start[1];
            p->delta[2] = p->destination[2] - p->start[2];
            p->update178 = Sword_UpdateThrustRetraction;
        }
        value = p->velocity[0];
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocity[0] = half;
        value = p->velocity[2];
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocity[2] = half;
    }
}
