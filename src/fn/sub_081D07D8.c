#include "global.h"
#include "background_controller.h"

struct Vec {
    s16 x, y, z, pad;
};
struct Projection {
    u8 pad[0x10];
    s32 x, y, z;
    u8 pad1C[0x1C];
    s32 height;
};
struct SceneNpc {
    u8 pad0[2];
    u8 field2, field3, state, pad5, prevState, pad7;
    u8 field8, field9;
    u8 padA[6];
    u32 field10, field14;
    u8 field18;
    u8 pad19[0x68 - 0x19];
    struct Vec position;
    u8 pad70[0xD4 - 0x70];
    u8 display[1];
    u8 padD5[0x2CC - 0xD5];
    void *update;
    u8 pad2D0[0x2F0 - 0x2D0];
    struct Projection projection;
    u8 pad33C[0x344 - 0x32C];
    u8 groundEffect[1];
};
struct Camera {
    u8 pad[0x44];
    s32 x;
};
void DisplayAnimation_SetFlags(void *, s32);
void GroundEffect_Initialize(void *, void *, s32);
void SceneNpc_UpdateProjectedVisible(void);
void SceneNpc_EnterProjectedIdle(void *unused, struct SceneNpc *npc)
{
    s32 zero, st;
    struct Projection *projection = &npc->projection;
    struct Vec *position;
    void (*update)(void);
    DisplayAnimation_SetFlags(npc->display, 0x10);
    position = &npc->position;
    projection->x = (position->x + gSceneCamera->x) << 12;
    projection->z = position->z << 12;
    zero = 0;
    projection->y = zero;
    projection->height = position->y << 12;
    projection->height = npc->position.y << 12;
    GroundEffect_Initialize(npc->groundEffect, &projection->x, 0);
    st = 0x22;
    npc->field2 = zero;
    npc->field3 = zero;
    npc->state = st;
    npc->field14 = zero;
    npc->field8 = 1;
    do {
        update = SceneNpc_UpdateProjectedVisible;
        npc->prevState = 0x1f;
        npc->update = update;
        npc->field10 = zero;
        npc->field9 = 1;
    } while (0);
    npc->field18 = zero;
}
