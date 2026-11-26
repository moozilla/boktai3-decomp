#include "scene/actor/scene_actor_components.h"
#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct SceneActorBody *SceneBody_GetById(u32);
s32 Div(s32, s32);
void Camera_NotifyMoveComplete(struct SceneCameraController *);
void Camera_UpdatePositionMode(struct SceneCameraController *camera)
{
    struct GameState *player = gGameState;
    struct PackedVec target;
    struct SceneActorBody *body;
    target.x = player->positionX + camera->offsetX;
    target.y = player->positionY + camera->offsetY;
    target.z = player->positionZ + camera->offsetZ;
    switch (camera->positionMode) {
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
            gCamera.position.x = camera->position.x;
            gCamera.position.y = camera->position.y;
            gCamera.position.z = camera->position.z;
            break;
        case 2:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->positionTimer - 1) + camera->position.x, camera->positionTimer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->positionTimer - 1) + camera->position.y, camera->positionTimer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->positionTimer - 1) + camera->position.z, camera->positionTimer);
            if (--camera->positionTimer <= 1) {
                camera->positionMode = 1;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 3: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gCamera.position.x = body->position.x + camera->offsetX;
                gCamera.position.y = body->position.y + camera->offsetY;
                gCamera.position.z = body->position.z + camera->offsetZ;
            }
            break;
        }
        case 4: {
            body = SceneBody_GetById(camera->targetId);
            if (body) {
                gCamera.position.x =
                    Div(gCamera.position.x * (camera->positionTimer - 1) + (body->position.x + camera->offsetX),
                        camera->positionTimer);
                gCamera.position.y =
                    Div(gCamera.position.y * (camera->positionTimer - 1) + (body->position.y + camera->offsetY),
                        camera->positionTimer);
                gCamera.position.z =
                    Div(gCamera.position.z * (camera->positionTimer - 1) + (body->position.z + camera->offsetZ),
                        camera->positionTimer);
                if (--camera->positionTimer <= 1) {
                    camera->positionMode = 3;
                    Camera_NotifyMoveComplete(camera);
                }
            }
            break;
        }
        case 5:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->positionTimer - 1) + target.x, camera->positionTimer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->positionTimer - 1) + target.y + 150, camera->positionTimer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->positionTimer - 1) + target.z, camera->positionTimer);
            if (--camera->positionTimer <= 1) {
                camera->positionMode = 0;
                Camera_NotifyMoveComplete(camera);
            }
            break;
        case 6:
            gCamera.position.x =
                Div(gCamera.position.x * (camera->positionTimer - 1) + camera->saved.x, camera->positionTimer);
            gCamera.position.y =
                Div(gCamera.position.y * (camera->positionTimer - 1) + camera->saved.y, camera->positionTimer);
            gCamera.position.z =
                Div(gCamera.position.z * (camera->positionTimer - 1) + camera->saved.z, camera->positionTimer);
            if (--camera->positionTimer <= 1) {
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
