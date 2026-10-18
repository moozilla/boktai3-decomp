#include "global.h"
typedef struct {
    u32 pad[2], flags;
    u8 gap[20];
    s16 x, y;
    u8 tail[60];
} Graphic;
void SelectionMenu_SetCallback(void *, void (*)(void));
void SelectionMenu_Choose(void);
void SelectionMenu_Close(void);
void Sound_PlaySong(int);
void Anim_LoadKeyframe(void *, void *, int, int);
void MessageWindow_SetBounds(int, int, int, int);
void Presentation_UpdateActorGraphic(void *, int);
void SelectionMenu_Back(u8 *p)
{
    Graphic *g = (Graphic *)(p + 0x1f4);
    if (p[0x1e]) {
        Sound_PlaySong(0xde);
        Anim_LoadKeyframe(g, p + 0x4c, 0x4d, 0);
        p[0x1e]--;
        ((Graphic *)(p + 0xc74 + p[0x1e] * 0x60))->flags |= 1;
        if (!p[0x1e])
            g->x = 16;
        else
            g->x = 64;
        g->y = 40;
        MessageWindow_SetBounds(2, 16, 30, 2);
        Presentation_UpdateActorGraphic(p, 0);
        SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    } else {
        Sound_PlaySong(0xde);
        SelectionMenu_SetCallback(p, SelectionMenu_Close);
    }
}
