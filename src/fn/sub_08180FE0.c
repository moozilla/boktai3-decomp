#include "global.h"

/* tentative: bouncing particle launched from the actor in its facing direction */
#define SceneCombatActor_InitDirectedParticle sub_08180FE0

struct Particle {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0x39e - 0xda];
    u8 phase;
    u8 pad39F[0x3b4 - 0x39f];
    u16 spritePosition[3];
    u8 pad3BA[0x3c4 - 0x3ba];
    u16 velocityX, velocityY, velocityZ;
    u8 pad3CA[0xb38 - 0x3ca];
    u8 kind, heading;
};
void SceneCombatActor_InitBouncingParticle(struct Particle *);
void SceneCombatActor_InitDirectedParticle(struct Particle *p)
{
    SceneCombatActor_InitBouncingParticle(p);
    p->velocityZ = 0;
    p->velocityY = 0;
    p->velocityX = 0;
    p->velocityY = 30;
    switch (((p->heading + 0x60) >> 6) & 3) {
        case 0:
            p->spritePosition[0] = p->position[0] - 200;
            p->spritePosition[1] = p->position[1] + 100;
            p->spritePosition[2] = p->position[2] - 200;
            p->phase = 0;
            p->velocityZ = 15;
            break;
        case 1:
            p->spritePosition[0] = p->position[0] - 50;
            p->spritePosition[1] = p->position[1] + 100;
            p->spritePosition[2] = p->position[2] - 200;
            p->phase = 0;
            p->velocityX = 0xfff1;
            break;
        case 2:
            p->spritePosition[0] = p->position[0] - 200;
            p->spritePosition[1] = p->position[1] + 100;
            p->spritePosition[2] = p->position[2] - 50;
            p->phase = 0;
            p->velocityZ = 0xfff1;
            break;
        case 3:
            p->spritePosition[0] = p->position[0] - 200;
            p->spritePosition[1] = p->position[1] + 100;
            p->spritePosition[2] = p->position[2] - 200;
            p->phase = 0;
            p->velocityX = 15;
            break;
    }
}
