#include "global.h"
#include "ram.h"
s32 Registry1745_GetSessionState(void);
void ScrollingDialogue_HideImmediately(void);
void Task_Kill(void *);
void Callback_InvokeActor(void *, u32);
extern u32 gEngineFlags;
struct SceneCallbackTask {
    u8 unknown[0xCE4];
    u32 callback;
};
s32 SceneTask_UpdateConditionalCallback(struct SceneCallbackTask *s)
{
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        if (Registry1745_GetSessionState() == 0) {
            ScrollingDialogue_HideImmediately();
            Task_Kill(s);
            return -1;
        }
    }
    if (s->callback != 0)
        Callback_InvokeActor(s, s->callback);
    return 1;
}
