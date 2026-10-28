#include "global.h"
#include "render.h"
#include "game_state.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Camera {
    u8 unknown0[0x1C];
    u16 moving;
    u16 unknown1E;
    u8 unknown20[0x38];
    struct CameraPosition saved;
    u8 unknown60[8];
    u16 mode, timer;
    s16 x, y, z;
    u16 unknown72;
    s16 offsetX, offsetY, offsetZ;
    u16 unknown7A;
    u16 targetId;
    u8 unknown7E[0x1E];
    struct CameraPosition *source;
    u32 *enabled;
};
struct Player {
    u8 unknown0[0x30];
    s16 x, y, z;
};
struct Body {
    u8 unknown0[12];
    s16 x, y, z;
};
struct Body *SceneBody_GetById(u32);
s32 Div(s32, s32);
void Camera_NotifyMoveComplete(struct Camera *);
void Camera_UpdatePositionMode(struct Camera *camera)
{
    struct Player *player = gGameState;
    struct PackedVec target;
    struct Body *body;
    target.x = player->x + camera->offsetX;
    target.y = player->y + camera->offsetY;
    target.z = player->z + camera->offsetZ;
    switch (camera->mode) {
        case 0: {
            struct CameraRenderState *state = &gCamera;
            if (state->mode == 0) {
                state->position.x = target.x;
                state->position.y = target.y + 150;
            } else {
                state->position.x = target.x;
                state->position.y = target.y - 150;
            }
            state->position.z = target.z;
            break;
        }
        case 1:
            gCamera.position.x = camera->x;
            gCamera.position.y = camera->y;
            gCamera.position.z = camera->z;
            break;
        case 2:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->timer - 1) + camera->x, camera->timer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->timer - 1) + camera->y, camera->timer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->timer - 1) + camera->z, camera->timer);
            if (--camera->timer <= 1) {
                camera->mode = 1;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 3: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gCamera.position.x = body->x + camera->offsetX;
                gCamera.position.y = body->y + camera->offsetY;
                gCamera.position.z = body->z + camera->offsetZ;
            }
            break;
        }
        case 4: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gCamera.position.x =
                    Div(gCamera.position.x * (camera->timer - 1) + (body->x + camera->offsetX),
                        camera->timer);
                gCamera.position.y =
                    Div(gCamera.position.y * (camera->timer - 1) + (body->y + camera->offsetY),
                        camera->timer);
                gCamera.position.z =
                    Div(gCamera.position.z * (camera->timer - 1) + (body->z + camera->offsetZ),
                        camera->timer);
                if (--camera->timer <= 1) {
                    camera->mode = 3;
                    Camera_NotifyMoveComplete(camera);
                }
            }
            break;
        }
        case 5:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->timer - 1) + target.x, camera->timer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->timer - 1) + target.y + 150, camera->timer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->timer - 1) + target.z, camera->timer);
            if (--camera->timer <= 1) {
                camera->mode = 0;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 6:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->timer - 1) + camera->saved.x, camera->timer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->timer - 1) + camera->saved.y, camera->timer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->timer - 1) + camera->saved.z, camera->timer);
            if (--camera->timer <= 1) {
                camera->moving = 0;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 7:
            if (*camera->enabled)
                gCamera.position = *camera->source;
            break;
    }
}
