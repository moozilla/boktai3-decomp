#include "global.h"
typedef struct {
    u32 pad[2], flags;
} Graphic;
extern u8 gUnk_08E60F80[][16];
int Inventory_GetConsumableId(u32);
void Presentation_ShowItemDescription(void *, u32);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_SetScript(u32);
void Presentation_UpdateItemGraphic(u8 *p, u32 fallback)
{
    int index = Inventory_GetConsumableId(p[0x1d]);
    if (index >= 0) {
        Presentation_ShowItemDescription(gUnk_08E60F80[index], *(u32 *)(p + 0xdac));
        ((Graphic *)(p + 0x5b4))->flags &= ~1;
        Anim_LoadKeyframe(p + 0x5b4, p + 0x6c, 0x18a, 0);
    } else {
        MessageWindow_Clear();
        if (!fallback)
            ScrollingDialogue_ClearScript();
        else
            ScrollingDialogue_SetScript(fallback);
        ((Graphic *)(p + 0x5b4))->flags |= 1;
    }
}
