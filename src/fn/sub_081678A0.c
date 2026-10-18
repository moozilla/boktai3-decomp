#define SelectionPanel_UpdateBlink    sub_081678A0
#define SelectionPanel_HideBlinkTiles sub_08167864
#define SelectionPanel_ShowBlinkTiles PauseMenu_InitHeaderTilemap
#include "global.h"
struct PanelOwner {
    u8 pad[0x418];
    u8 state;
};
struct SelectionPanel {
    u8 pad[0xA40];
    struct PanelOwner *owner;
    u8 padA44[0x56];
    u8 visible;
    u8 blinkTimer;
};
struct PanelClock {
    u8 pad[0x42];
    s16 active;
};
extern struct PanelClock *gUnk_02000710;
void sub_08167864(struct SelectionPanel *);
void PauseMenu_InitHeaderTilemap(struct SelectionPanel *);
void sub_081678A0(struct SelectionPanel *p)
{
    if (p->owner->state != 6) {
        if (gUnk_02000710->active > 0) {
            if (p->blinkTimer <= 29)
                sub_08167864(p);
            else
                PauseMenu_InitHeaderTilemap(p);
            if (++p->blinkTimer > 59)
                p->blinkTimer = 0;
        } else
            sub_08167864(p);
    }
}
