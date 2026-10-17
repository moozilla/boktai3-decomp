#include "global.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    u8 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

extern u8 *gUnk_0200047C;
extern u32 gUnk_030051C8;

void sub_08020D54(void *, void *);
void Trigger_ConsumePending(void *, s32);
void SceneCommands_CreateEffect(void *, void *, void *);
void sub_08020F6C(void *, void *, void *);
void sub_08020FE4(void *, void *, void *);
void SceneCommands_Fade(void *, void *, void *);
void SceneCommands_FadePalette(void *, void *, void *);
void SceneCommands_BlendPalette(void *, void *, void *);
void sub_080211B0(void *, void *, void *);
void sub_080211C0(void *, void *, void *);
void sub_080211C8(void *, void *, void *);

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
            sub_08020D54(entry, message);
            switch (message[6]) {
                case 0:
                    SceneCommands_CreateEffect(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 1:
                    sub_08020F6C(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 2:
                    sub_08020FE4(p, entry, message);
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
                    sub_080211B0(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 7:
                    sub_080211C0(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 8:
                    sub_080211C8(p, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 9:
                    gUnk_030051C8 = 0;
                    Trigger_ConsumePending(entry, 1);
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
    return 0;
}
