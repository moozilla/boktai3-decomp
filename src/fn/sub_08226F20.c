#include "global.h"
#include "game_state.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Camera {
    u8 unknown0[0x1C];
    u16 moving;
    u16 unknown1E;
    u8 unknown20[0x38];
    struct Vec saved;
    u8 unknown60[8];
    u16 mode, timer;
    s16 x, y, z;
    u16 unknown72;
    s16 offsetX, offsetY, offsetZ;
    u16 unknown7A;
    u16 targetId;
    u8 unknown7E[0x1E];
    struct Vec *source;
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
struct CameraState {
    struct Vec position;
    u8 unknown8[10];
    s16 facing;
};
extern struct CameraState gUnk_030042A0;
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
            struct CameraState *state = &gUnk_030042A0;
            if (state->facing == 0) {
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
            gUnk_030042A0.position.x = camera->x;
            gUnk_030042A0.position.y = camera->y;
            gUnk_030042A0.position.z = camera->z;
            break;
        case 2:
            gUnk_030042A0.position.x =
                Div(gUnk_030042A0.position.x * (camera->timer - 1) + camera->x, camera->timer);
            gUnk_030042A0.position.y =
                Div(gUnk_030042A0.position.y * (camera->timer - 1) + camera->y, camera->timer);
            gUnk_030042A0.position.z =
                Div(gUnk_030042A0.position.z * (camera->timer - 1) + camera->z, camera->timer);
            if (--camera->timer <= 1) {
                camera->mode = 1;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 3: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gUnk_030042A0.position.x = body->x + camera->offsetX;
                gUnk_030042A0.position.y = body->y + camera->offsetY;
                gUnk_030042A0.position.z = body->z + camera->offsetZ;
            }
            break;
        }
        case 4: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gUnk_030042A0.position.x = Div(gUnk_030042A0.position.x * (camera->timer - 1) +
                                                   (body->x + camera->offsetX),
                                               camera->timer);
                gUnk_030042A0.position.y = Div(gUnk_030042A0.position.y * (camera->timer - 1) +
                                                   (body->y + camera->offsetY),
                                               camera->timer);
                gUnk_030042A0.position.z = Div(gUnk_030042A0.position.z * (camera->timer - 1) +
                                                   (body->z + camera->offsetZ),
                                               camera->timer);
                if (--camera->timer <= 1) {
                    camera->mode = 3;
                    Camera_NotifyMoveComplete(camera);
                }
            }
            break;
        }
        case 5:
            gUnk_030042A0.position.x =
                Div(gUnk_030042A0.position.x * (camera->timer - 1) + target.x, camera->timer);
            gUnk_030042A0.position.y =
                Div(gUnk_030042A0.position.y * (camera->timer - 1) + target.y + 150, camera->timer);
            gUnk_030042A0.position.z =
                Div(gUnk_030042A0.position.z * (camera->timer - 1) + target.z, camera->timer);
            if (--camera->timer <= 1) {
                camera->mode = 0;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 6:
            gUnk_030042A0.position.x = Div(
                gUnk_030042A0.position.x * (camera->timer - 1) + camera->saved.x, camera->timer);
            gUnk_030042A0.position.y = Div(
                gUnk_030042A0.position.y * (camera->timer - 1) + camera->saved.y, camera->timer);
            gUnk_030042A0.position.z = Div(
                gUnk_030042A0.position.z * (camera->timer - 1) + camera->saved.z, camera->timer);
            if (--camera->timer <= 1) {
                camera->moving = 0;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 7:
            if (*camera->enabled)
                gUnk_030042A0.position = *camera->source;
            break;
    }
}
