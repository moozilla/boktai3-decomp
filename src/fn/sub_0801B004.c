#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct {
    u8 pad[2], level, tail[29];
} Actor;
typedef struct {
    u32 pad[2], flags;
    u8 gap[20];
    s16 x, y;
    u8 tail[60];
} Graphic;
extern u16 gUnk_03005260[];
extern u8 *gUnk_02000710;
extern Actor gUnk_08E6092C[];
int SelectionMenu_ConsumeCallbackFlag(void *);
void Anim_LoadKeyframe(void *, void *, int, int);
void ScrollingDialogue_ClearScript(void);
void Presentation_SetActorIconFrame(void *, void *, void *);
void MessageWindow_SetBounds(int, int, int, int);
void MessageWindow_SetMessageTable(u32);
int Inventory_HasItem(int);
void MessageWindow_ShowMessage(int);
void Sound_PlaySong(int);
void SelectionMenu_HandleHorizontalInput(void *);
void SelectionMenu_Back(void *);
void SelectionMenu_SetCallback(void *, void (*)(void));
void SelectionMenu_Confirm(void);
void SelectionMenu_ReviewChoice(u8 *p)
{
    Graphic *g;
    if (SelectionMenu_ConsumeCallbackFlag(p)) {
        g = (Graphic *)(p + 0xd34);
        Anim_LoadKeyframe(p + 0x194, p + 0x4c, 0x4d, 0);
        ScrollingDialogue_ClearScript();
        {
            u8 *result = p + 0xd94;
            Presentation_SetActorIconFrame(p, g, result);
        }
        g->flags &= ~1;
        g->x = 104;
        g->y = 40;
        p[0x20] = 0;
        Anim_LoadKeyframe(p + 0x254, p + 0x4c, 0x4b, 0);
        *(u32 *)(p + 0x25c) &= ~1;
        MessageWindow_SetBounds(0, 16, 30, 2);
        MessageWindow_SetMessageTable(*(u32 *)(p + 0xdb0));
        {
            Actor *actor = &gUnk_08E6092C[p[0xd94]];
            if (*(s16 *)(gUnk_02000710 + 0x40) >= actor->level || Inventory_HasItem(0x29))
                MessageWindow_ShowMessage(1);
            else
                MessageWindow_ShowMessage(2);
        }
        ((Graphic *)(p + 0x5b4))->flags |= 1;
        ScrollingDialogue_ClearScript();
    }
    if (gUnk_03005260[0] & 4) {
        if (gUnk_03005260[1] & 1) {
            if (++p[0x24] > 2)
                p[0x24] = 0;
            Sound_PlaySong(0xdc);
        }
    } else {
        SelectionMenu_HandleHorizontalInput(p);
        {
            int keys = gUnk_03005260[1], mask;
            mask = 1;
            asm("" : "+r"(mask));
            if (mask & keys) {
                if (!p[0x20]) {
                    Sound_PlaySong(0xdd);
                    SelectionMenu_SetCallback(p, SelectionMenu_Confirm);
                    return;
                }
                goto back;
            } else if (2 & keys) {
            back:
                ((Graphic *)(p + 0xd34))->flags |= mask;
                *(u32 *)(p + 0x25c) |= mask;
                SelectionMenu_Back(p);
                return;
            }
            asm("" ::"r"(keys));
        }
    }
    (*(u32 *)(p + 0x28))++;
}
