#include "global.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    u16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    u16 x, y, z, w;
};

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
void Camera_SetMessageCompletionMode(u8 *);
void Camera_ConsumePendingMessage(u8 *);
void Camera_Reset(void);
void Camera_SnapTo(struct Vec *);
void Camera_PanTo(s32, struct Vec *);
void Camera_ReturnToDefault(s32);
void Camera_SetSway(s32, s32, s32);
void Camera_FollowPlayer(s32);

void Camera_DispatchCommands(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x20);
    s32 i;
    struct Vec snap;
    struct Vec pan;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]])
        do {
            u16 *message;
            u8 *controller = gSceneEventDispatchState;
            message = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, message);
            switch (((u8 *)message)[6]) {
                case 0:
                    Camera_SetMessageCompletionMode(p);
                    Camera_Reset();
                    Camera_ConsumePendingMessage(p);
                    break;
                case 1:
                    Camera_SetMessageCompletionMode(p);
                    snap.x = message[4];
                    snap.y = message[5];
                    snap.z = message[6];
                    Camera_SnapTo(&snap);
                    Camera_ConsumePendingMessage(p);
                    break;
                case 2:
                    Camera_SetMessageCompletionMode(p);
                    pan.x = message[4];
                    pan.y = message[5];
                    pan.z = message[6];
                    Camera_PanTo((s16)message[7], &pan);
                    break;
                case 3:
                    Camera_SetMessageCompletionMode(p);
                    Camera_ReturnToDefault((s16)message[4]);
                    break;
                case 4:
                    Camera_SetMessageCompletionMode(p);
                    Camera_SetSway((s16)message[4], (s16)message[5], (s16)message[6]);
                    break;
                case 5:
                    Camera_SetMessageCompletionMode(p);
                    Camera_FollowPlayer((s16)message[4]);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
}
