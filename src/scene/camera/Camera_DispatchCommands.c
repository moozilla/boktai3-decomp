#include "scene/camera/scene_camera_controller.h"
#include "scene/camera/scene_camera_vector_layout.h"

void Camera_SetMessageCompletionMode(struct SceneCameraController *);
void Camera_ConsumePendingMessage(struct SceneCameraController *);
void Camera_Reset(void);
void Camera_SnapTo(struct SceneCameraVector *);
void Camera_PanTo(s32, struct SceneCameraVector *);
void Camera_ReturnToDefault(s32);
void Camera_SetSway(s32, s32, s32);
void Camera_FollowPlayer(s32);

void Camera_DispatchCommands(struct SceneCameraController *camera)
{
    struct SceneEventReceiver *entry = &camera->receiver.queue;
    s32 i;
    struct SceneCameraVector snap;
    struct SceneCameraVector pan;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState->phase])
        do {
            struct SceneEventCommand *message;
            const u16 *packet;
            struct SceneEventDispatchState *controller = gSceneEventDispatchState;
            message = entry->pending[controller->phase][i];
            SceneMessages_SetPendingTrigger(entry, message);
            /* Camera arguments follow the eight-byte message header. */
            packet = (const void *)message;
            switch (message->kind) {
                case 0:
                    Camera_SetMessageCompletionMode(camera);
                    Camera_Reset();
                    Camera_ConsumePendingMessage(camera);
                    break;
                case 1:
                    Camera_SetMessageCompletionMode(camera);
                    snap.x = packet[4];
                    snap.y = packet[5];
                    snap.z = packet[6];
                    Camera_SnapTo(&snap);
                    Camera_ConsumePendingMessage(camera);
                    break;
                case 2:
                    Camera_SetMessageCompletionMode(camera);
                    pan.x = packet[4];
                    pan.y = packet[5];
                    pan.z = packet[6];
                    Camera_PanTo((s16)packet[7], &pan);
                    break;
                case 3:
                    Camera_SetMessageCompletionMode(camera);
                    Camera_ReturnToDefault((s16)packet[4]);
                    break;
                case 4:
                    Camera_SetMessageCompletionMode(camera);
                    Camera_SetSway((s16)packet[4], (s16)packet[5], (s16)packet[6]);
                    break;
                case 5:
                    Camera_SetMessageCompletionMode(camera);
                    Camera_FollowPlayer((s16)packet[4]);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState->phase]);
}
