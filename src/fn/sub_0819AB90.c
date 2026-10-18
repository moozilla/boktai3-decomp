#include "global.h"

#define SceneOrbitHazard_UpdateOrbit sub_0819AB90
#define SceneOrbitHazard_CalcOrbitPosition sub_0819AAF4

struct SceneOrbitPosition {
    s16 x, y, z, unused;
};

/* Scene orbit hazard identity is tentative. */
struct SceneOrbitHazard {
    u8 unk0[0x18];
    u8 effect[0xB0];
    struct SceneOrbitPosition *anchor;
    struct SceneOrbitPosition position;
    u8 unkD4[8];
    s16 effectParameter;
    u16 timer;
    u8 mode;
    u8 phase;
    u8 angle;
};

void sub_081806A0(void *, s32, s32, s32);
void sub_081805F0(void *, s32, struct SceneOrbitPosition *);
void sub_0819AAF4(struct SceneOrbitHazard *, struct SceneOrbitPosition *);

void sub_0819AB90(struct SceneOrbitHazard *hazard)
{
    struct SceneOrbitPosition position;
    switch (hazard->phase) {
    case 0:
        hazard->angle = 0;
        hazard->position = *hazard->anchor;
        hazard->phase++;
        sub_081806A0(hazard->effect, hazard->effectParameter, 10, 10);
        break;
    case 1:
        if (hazard->timer > 119)
            hazard->phase = 255;
        break;
    }
    hazard->angle += 16;
    sub_0819AAF4(hazard, &position);
    sub_081805F0(hazard->effect, hazard->angle, &position);
}
