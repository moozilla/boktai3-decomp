#include "global.h"
#include "render.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"

struct Vec {
    s16 x, y, z;
};

struct Camera {
    u8 pad[0x18];
    u32 id;
    u16 f1c;
    u16 f1e;
    u8 receiver[0x54 - 0x20];
    u16 f54;
    u16 f56;
    s16 x, y, z;
    u8 pad5e[0x7E - 0x5E];
    u16 f7E;
    u16 f80;
    u16 f82;
    u8 b84;
    u8 b85;
    u16 h86;
    u16 h88;
    u8 pad8a[0x8C - 0x8A];
    u16 f8C;
    u8 pad8e[0x98 - 0x8E];
    u32 f98;
};

int SceneMessages_RegisterReceiver(u8 *p, int id, int kind);

static inline int project(int input)
{
    int v = input * 3;
    return v >= 0 ? v >> 5 : -(-v >> 5);
}

static inline void updateScroll(void)
{
    struct Vec *out = &gCameraOrigin;
    struct CameraPosition *in = &gCamera.position;
    int xy, height, x = in->x, z = in->z;
    out->x = project(x - z);
    xy = project(x + z);
    height = project(in->y);
    out->y = xy - height;
    out->z = xy + height;
}

/* Camera setup: start at the player's saved position (raised by 150), register
   as the kind 6 (camera) cutscene receiver and become gActiveCamera. */
s32 Camera_Initialize(struct Camera *c, u32 id)
{
    u8 *gs;

    c->id = id;
    c->f1c = 0;
    c->f54 = 0;
    gs = gGameState;
    c->x = *(u16 *)(gs + 0x30);
    c->y = *(u16 *)(gs + 0x32) + 0x96;
    c->z = *(u16 *)(gs + 0x34);
    c->f8C = 0;
    gCamera.position.x = c->x;
    gCamera.position.y = c->y;
    gCamera.position.z = c->z;
    updateScroll();
    c->b84 = 0;
    c->b85 = 0;
    c->h86 = 0;
    c->h88 = 0;
    c->f7E = 0;
    c->f80 = 0;
    c->f82 = 0;
    c->f98 = 0;
    updateScroll();
    if (!(gEngineFlags & 0x800))
        SceneMessages_RegisterReceiver((u8 *)c + 0x20, c->id, 6);
    gActiveCamera = c;
    return 0;
}
