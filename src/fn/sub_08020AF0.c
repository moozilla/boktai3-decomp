#include "global.h"
extern u8 *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(void *, void *);
void SceneMessages_RunScript(void *, void *, void *);
void SceneMessages_SetDelay(void *, void *, void *);
void SceneMessages_SetSignal(void *, void *, void *);
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    void *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;
int SceneMessages_Dispatch(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x78);
    int i;
    i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]]) {
        do {
            u8 *message;
            u8 *controller = gSceneEventDispatchState;
            message = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, message);
            switch (message[6]) {
                case 0:
                    SceneMessages_RunScript(p, entry, message);
                    break;
                case 1:
                    SceneMessages_SetDelay(p, entry, message);
                    break;
                case 2:
                    SceneMessages_SetSignal(p, entry, message);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
    }
    return 0;
}
