#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
#include "engine/ram.h"

struct Vec {
    s16 x, y, z;
};

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
s32 Camera_Initialize(struct SceneCameraController *c, u32 id)
{
    struct GameState *gs;

    c->id = id;
    c->moving = 0;
    c->secondaryMode = 0;
    gs = gGameState;
    c->saved.x = gs->positionX;
    c->saved.y = gs->positionY + 0x96;
    c->saved.z = gs->positionZ;
    c->boundsMode = 0;
    gCamera.position.x = c->saved.x;
    gCamera.position.y = c->saved.y;
    gCamera.position.z = c->saved.z;
    updateScroll();
    c->field84 = 0;
    c->field85 = 0;
    c->field86 = 0;
    c->field88 = 0;
    c->field7E = 0;
    c->field80 = 0;
    c->field82 = 0;
    c->rate = 0;
    updateScroll();
    if (!(gEngineFlags & 0x800))
        SceneMessages_RegisterReceiver(&c->receiver, c->id, 6);
    gActiveCamera = (void *)c;
    return 0;
}
