#include "global.h"

struct ActorMarker {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0x39e - 0xda];
    u8 state;
    u8 pad39F[0x3b4 - 0x39f];
    u16 offsetPosition[3];
    u8 pad3BA[0x3c4 - 0x3ba];
    u16 counters[3];
    u8 pad3CA[0xb39 - 0x3ca];
    u8 direction;
};
void SceneCombatActor_InitBouncingParticle(struct ActorMarker *);
void ActorMarker_InitOffsetPosition(struct ActorMarker *p)
{
    SceneCombatActor_InitBouncingParticle(p);
    p->counters[2] = 0;
    p->counters[1] = 0;
    p->counters[0] = 0;
    switch (((p->direction + 0x60) >> 6) & 3) {
        case 1:
            p->offsetPosition[0] = p->position[0] - 50;
            p->offsetPosition[1] = p->position[1] + 100;
            p->offsetPosition[2] = p->position[2] - 200;
            p->state = 0;
            break;

        case 2:
            p->offsetPosition[0] = p->position[0] - 200;
            p->offsetPosition[1] = p->position[1] + 100;
            p->offsetPosition[2] = p->position[2] - 50;
            p->state = 0;
            break;

        case 3:
            p->offsetPosition[0] = p->position[0] - 200;
            p->offsetPosition[1] = p->position[1] + 100;
            p->offsetPosition[2] = p->position[2] - 200;
            p->state = 0;
            break;

        case 0:
            p->offsetPosition[0] = p->position[0] - 200;
            p->offsetPosition[1] = p->position[1] + 100;
            p->offsetPosition[2] = p->position[2] - 200;
            p->state = 0;
            break;
    }
}
