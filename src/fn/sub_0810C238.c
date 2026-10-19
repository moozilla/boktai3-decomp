#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.8%
// foundry draft (worker best 86.8%); what differs is mostly register allocation and instruction order
#define LinkPanel_UpdateAnimations sub_0810C238
#define LinkPanel_SelectActiveAnimation sub_0810C184
#define LinkPanel_SelectReadyAnimation RegistryF246_ShowSlotReadyFrame
#define LinkPanel_SelectInactiveAnimation RegistryF246_ShowSlotDefaultFrame
#define gLinkPanelState gUnk_020004B4

struct LinkPanelPeer { u8 pad0[0x53]; u8 active; };
struct LinkPanelEntry { u8 state; u8 pad1; u8 timer; u8 pad3[0x5F9]; };
struct LinkPanel { u8 pad0[0x20]; u8 animations[7][0x60]; u8 pad2C0[0x20]; u8 data[0x20]; u16 selected[7]; s16 previous[7]; u8 pad31C[0x624]; struct LinkPanelEntry entries[2]; };
extern struct LinkPanelPeer *gUnk_020004B4;
void sub_0810C184(struct LinkPanel *, s32);
void RegistryF246_ShowSlotReadyFrame(struct LinkPanel *, s32);
void RegistryF246_ShowSlotDefaultFrame(struct LinkPanel *, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
s32 Anim_Step(void *, void *);
static inline u8 valid(s32 i) { if (i < 0) return 0; if (i > 1) return 0; return 1; }
void sub_0810C238(struct LinkPanel *p)
{
    s32 i = 0;
    
    struct LinkPanelPeer *peer;
    void *anim = p->animations;
    register u32 j = 0;
    struct LinkPanelEntry *entry = p->entries;
    u8 *timer = &p->entries[0].timer;
    for (; i <= 1; i++) {
        peer = gUnk_020004B4;
        if (peer && peer->active == 1 && valid(i) && (u8 *)peer + (0x120 + i * 0x40)) {
            if (entry->timer > 30) {
                if (entry->state > 1) sub_0810C184(p, i);
                else if (entry->state == 1) RegistryF246_ShowSlotReadyFrame(p, i);
                else goto inactive;
                goto step;
            } else goto increment;
        } else { inactive: RegistryF246_ShowSlotDefaultFrame(p, i); goto step; }
        increment: *timer = entry->timer + 1;
        step:
        if (p->selected[j >> 1] != p->previous[j >> 1]) {
            Anim_SelectWidgetGlyph(anim, p->data, p->selected[j >> 1]);
            p->previous[j >> 1] = p->selected[j >> 1];
        }
        Anim_Step(anim, p->data);
        anim = (u8 *)anim + 0x60;

        j += 2;
        entry++;
        timer += 0x5FC;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810C238);
#endif
