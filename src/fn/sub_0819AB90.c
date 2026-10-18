#include "global.h"

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

void SpriteComponent_ConfigureCollider(void *, s32, s32, s32);
void SpriteComponent_SetFramePosition(void *, s32, struct SceneOrbitPosition *);
void DirectionalActor_GetForwardPosition(struct SceneOrbitHazard *, struct SceneOrbitPosition *);

void SceneOrbitHazard_UpdateOrbit(struct SceneOrbitHazard *hazard)
{
    struct SceneOrbitPosition position;
    switch (hazard->phase) {
        case 0:
            hazard->angle = 0;
            hazard->position = *hazard->anchor;
            hazard->phase++;
            SpriteComponent_ConfigureCollider(hazard->effect, hazard->effectParameter, 10, 10);
            break;
        case 1:
            if (hazard->timer > 119)
                hazard->phase = 255;
            break;
    }
    hazard->angle += 16;
    DirectionalActor_GetForwardPosition(hazard, &position);
    SpriteComponent_SetFramePosition(hazard->effect, hazard->angle, &position);
}
