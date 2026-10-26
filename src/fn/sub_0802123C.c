#include "global.h"
#include "render.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    u8 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

extern u8 *gUnk_0200047C;

void SceneMessages_SetPendingTrigger(void *, void *);
void Trigger_ConsumePending(void *, s32);
void SceneCommands_CreateEffect(void *, void *, void *);
void SceneCommands_ClearEffect(void *, void *, void *);
void SceneCommands_CreateOrbitingParticles(void *, void *, void *);
void SceneCommands_Fade(void *, void *, void *);
void SceneCommands_FadePalette(void *, void *, void *);
void SceneCommands_BlendPalette(void *, void *, void *);
void SceneCommands_SetPosition(void *, void *, void *);
void SceneCommands_ClearPosition(void *, void *, void *);
void SceneCommands_ConfigureBlendWindow(void *, void *, void *);

s32 SceneCommands_Dispatch(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x18);
    s32 i;

    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            u8 *message;
            u8 *controller = gUnk_0200047C;
            message = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, message);
            switch (message[6]) {
                case 0:
                    SceneCommands_CreateEffect(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 1:
                    SceneCommands_ClearEffect(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 2:
                    SceneCommands_CreateOrbitingParticles(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 3:
                    SceneCommands_Fade(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 4:
                    SceneCommands_FadePalette(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 5:
                    SceneCommands_BlendPalette(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 6:
                    SceneCommands_SetPosition(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 7:
                    SceneCommands_ClearPosition(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 8:
                    SceneCommands_ConfigureBlendWindow(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 9:
                    gObjPaletteEffect = 0;
                    Trigger_ConsumePending(entry, 1);
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
    return 0;
}
