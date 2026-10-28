#include "global.h"
#include "area_effect_link.h"
struct LinkPanelPeer {
    u8 pad0[0x53];
    u8 active;
};
struct LinkPanelEntry {
    u8 state;
    u8 pad1;
    u8 timer;
    u8 pad3[0x5F9];
};
struct LinkPanel {
    u8 pad0[0x20];
    u8 animations[7][0x60];
    u8 pad2C0[0x20];
    u8 data[0x20];
    u16 selected[7];
    s16 previous[7];
    u8 pad31C[0x624];
    struct LinkPanelEntry entries[2];
};
void LinkPanel_SelectActiveAnimation(struct LinkPanel *, s32);
void RegistryF246_ShowSlotReadyFrame(struct LinkPanel *, s32);
void RegistryF246_ShowSlotDefaultFrame(struct LinkPanel *, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
s32 Anim_Step(void *, void *);
static inline u8 valid(s32 i)
{
    if (i < 0)
        return 0;
    if (i > 1)
        return 0;
    return 1;
}
// confidence=low: per link slot, picks the slot frame from the peer state, then steps its glyph animation.
void LinkPanel_UpdateAnimations(struct LinkPanel *p)
{
    s32 i;
    struct LinkPanelPeer *peer;
    for (i = 0; i <= 1; i++) {
        peer = gLinkSession;
        if (peer && peer->active == 1 && valid(i) && (u8 *)peer + (0x120 + i * 0x40)) {
            if (p->entries[i].timer > 30) {
                if (p->entries[i].state > 1)
                    LinkPanel_SelectActiveAnimation(p, i);
                else if (p->entries[i].state == 1)
                    RegistryF246_ShowSlotReadyFrame(p, i);
                else
                    goto inactive;
            } else
                goto increment;
        } else {
        inactive:
            RegistryF246_ShowSlotDefaultFrame(p, i);
        }
        goto step;
    increment:
        p->entries[i].timer++;
    step:
        if (p->selected[i] != p->previous[i]) {
            Anim_SelectWidgetGlyph(p->animations[i], p->data, p->selected[i]);
            p->previous[i] = p->selected[i];
        }
        Anim_Step(p->animations[i], p->data);
    }
}
