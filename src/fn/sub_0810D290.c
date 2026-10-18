#include "global.h"
struct PanelSequence {
    u8 pad0[0x18];
    void *map;
    u8 pad1C[0x308];
    u16 indicators[1];
    u8 pad326[0x61A];
    u8 panels[1][0x850];
    u8 pad1190[0x258];
    void (*update)(struct PanelSequence *);
    u8 pad13EC[0x10];
    u8 transition;
    u8 pad13FD[0xF];
    u8 entering;
    u8 pad140D;
    u16 timer;
};
void BgGfx_LoadTilemapSimple(u32,u32,u32,u32,u32);
void Bg_SetLayerMap(u32,void *,u16);
void LinkPanel_DrawPeerTimes(struct PanelSequence *);
void CharacterModel_UpdatePairedAnimations(void *, int);
void PaletteFade_CycleMenuColors(void *);
void PanelTransition_UpdateExit(struct PanelSequence *);
void PanelSequence_UpdateDelay(struct PanelSequence *s)
{
    int i;
    u8 *panel;
    if (s->entering) {
        Bg_SetLayerMap(3, s->map, 2);
        BgGfx_LoadTilemapSimple(3,0,0,0,0);
        LinkPanel_DrawPeerTimes(s);
        s->timer = 0;
        s->entering = 0;
    }
    i = 0;
    panel = s->panels[0];
    do {
        CharacterModel_UpdatePairedAnimations(panel, i);
        panel += 0x850;
        i++;
    } while (i < 2);
    PaletteFade_CycleMenuColors(s->indicators);
    if (s->timer < 240) {
        s->timer++;
    } else {
        void (*next)(struct PanelSequence *) = PanelTransition_UpdateExit;
        u32 one = 1;
        s->update = next;
        s->entering = one;
        s->transition = one;
    }
}
