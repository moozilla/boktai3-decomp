#include "global.h"
typedef struct {
    u32 pad[2], flags;
} Graphic;
u8 *Inventory_GetEquipmentEntry(u32);
void ActionRecord_CopyFromEntry(u8 *, void *);
void MessageWindow_SetMessageTable(u32);
void Presentation_ApplyGraphicData(void *, void *);
void Presentation_ApplyGraphicSettings(u8 *, u32);
u32 Presentation_SelectGraphicResource(u8 *);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_SetScript(u32);
void Presentation_UpdateActorGraphic(u8 *p, u32 fallback)
{
    u8 *actor = Inventory_GetEquipmentEntry(p[0x1d]);
    u8 data[32];
    if (*actor != 255) {
        ActionRecord_CopyFromEntry(actor, data);
        MessageWindow_SetMessageTable(*(u32 *)(p + 0xda8));
        Presentation_ApplyGraphicData(data, p + 0x44);
        Presentation_ApplyGraphicSettings(actor, *(u32 *)(p + 0xda8));
        {
            u32 resource = Presentation_SelectGraphicResource(actor);
            ((Graphic *)(p + 0x5b4))->flags &= ~1;
            Anim_LoadKeyframe(p + 0x5b4, p + 0x6c, (u16)resource, 0);
        }
    } else {
        MessageWindow_Clear();
        if (!fallback)
            ScrollingDialogue_ClearScript();
        else
            ScrollingDialogue_SetScript(fallback);
        ((Graphic *)(p + 0x5b4))->flags |= 1;
    }
}
