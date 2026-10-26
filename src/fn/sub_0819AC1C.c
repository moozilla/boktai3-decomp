#include "global.h"

#define gSineTable gUnk_086149C4

struct SceneOrbitPosition {
    s16 x, y, z, unused;
};

/* Scene orbit hazard identity is tentative. */
struct SceneOrbitHazard {
    u8 unk0[0x18];
    u8 effect[0xB0];
    struct SceneOrbitPosition *anchor;
    struct SceneOrbitPosition position;
    struct SceneOrbitPosition velocity;
    s16 effectParameter;
    u16 timer;
    u8 mode;
    u8 phase;
    u8 angle;
};

struct SceneOrbitTarget {
    u8 unk0[0x30];
    s16 x, y, z;
};

extern struct SceneOrbitTarget *gGameState;
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void SpriteComponent_Suspend(u8 *);
void SpriteComponent_SetFramePosition(void *, s32, struct SceneOrbitPosition *);
void DirectionalActor_GetForwardPosition(struct SceneOrbitHazard *, struct SceneOrbitPosition *);

static inline s32 SceneOrbitHazard_ScaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    else
        return -((-value) >> 12);
}

static inline void SceneOrbitHazard_AddVelocity(struct SceneOrbitPosition *position,
                                                struct SceneOrbitPosition *velocity)
{
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
}

void SceneOrbitHazard_UpdateTravel(struct SceneOrbitHazard *hazard)
{
    struct SceneOrbitPosition position;
    s32 angle, sine, cosine;
    switch (hazard->phase) {
        case 0:
            angle =
                Math_Atan2(gGameState->x - hazard->anchor->x, gGameState->z - hazard->anchor->z);
            sine = gSineTable[angle & 255];
            cosine = gSineTable[(angle + 64) & 255];
            hazard->velocity.x = SceneOrbitHazard_ScaleVelocity(cosine * 15);
            hazard->velocity.y = 0;
            hazard->velocity.z = SceneOrbitHazard_ScaleVelocity(sine * 15);
            hazard->phase++;
            break;
        case 1:
            if (hazard->timer > 179) {
                SpriteComponent_Suspend(hazard->effect);
                hazard->phase = 255;
                return;
            }
            break;
    }
    hazard->angle += 16;
    SceneOrbitHazard_AddVelocity(&hazard->position, &hazard->velocity);
    DirectionalActor_GetForwardPosition(hazard, &position);
    SpriteComponent_SetFramePosition(hazard->effect, hazard->angle, &position);
}
