#include "global.h"
struct LinkSlot {
    u8 cur, prev, state;
    u8 pad3[0x5fc - 3];
};
struct LinkPanel {
    u8 pad0[0x18];
    void *layerMap;
    u8 pad1C[0x268 - 0x1c];
    u32 flags;
    u8 pad26C[0x324 - 0x26c];
    u8 palette[0x940 - 0x324];
    u8 slotData[0xf40 - 0x940];
    void (*update)(struct LinkPanel *);
    u32 script;
    u8 padF48[8];
    s32 role;
    s32 peerIndex;
    u8 exiting;
    u8 message;
    u8 oldNumber;
    u8 padF5B;
    u8 delay;
    u8 padF5D[0xf6c - 0xf5d];
    u8 redraw;
    u8 padF6D;
    u16 timer;
};
extern u8 *gUnk_020004B4;
extern u16 gUnk_03005260[];
void Bg_SetLayerMap(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void RegistryF246_ShowSlotDefaultFrame(struct LinkPanel *, s32);
void ScrollingDialogue_ClearScript(void);
s32 Script_ParseStringRef(u32);
const u8 *Text_LookupString(s32);
void ScrollingDialogue_SetScript(const u8 *);
s32 Serial_GetRole(void);
void SlotMenu_UpdateLastEntry(struct LinkPanel *);
void LinkPanel_UpdateAnimations(struct LinkPanel *);
void PaletteFade_CycleMenuColors(void *);
void LinkPanel_UpdateStatusMessage(struct LinkPanel *);
void SlotMenu_UpdateExitTransition(struct LinkPanel *);
void Sound_PlaySong(u32);

static inline s32 peerStatus(s32 i)
{
    if (!gUnk_020004B4)
        return 0;
    {
        u8 *statuses = gUnk_020004B4 + 0x64;
        return statuses[i];
    }
}
static inline u8 unavailable(void)
{
    if (gUnk_020004B4 && gUnk_020004B4[0x2c] == 1)
        return 1;
    return 0;
}

static inline s32 GetPeer(void)
{
    u8 *st = gUnk_020004B4;
    s32 r = -1;
    if (st)
        r = *(u16 *)(st + 0x1a);
    return r;
}
static inline void SetUpdate(struct LinkPanel *p, void (*fp)(struct LinkPanel *), u8 st)
{
    p->update = fp;
    p->redraw = 1;
    p->exiting = st;
}

void sub_0810C7F4(struct LinkPanel *p)
{
    s32 i;
    struct LinkSlot *s;
    u32 zero;
    if (p->redraw) {
        p->message = 0xff;
        Bg_SetLayerMap(3, p->layerMap, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        p->timer = 0;
        p->redraw = 0;
        for (i = 0, zero = 0, s = (struct LinkSlot *)p->slotData; i <= 1; s++, i++) {
            s->prev = s->cur;
            s->cur = zero;
            s->state = zero;
            RegistryF246_ShowSlotDefaultFrame(p, i);
        }

        p->padF5B = 0;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 2));
        p->message = 2;
        p->delay = 2;
    }
    p->role = Serial_GetRole();
    p->peerIndex = GetPeer();
    {
        u32 zero2;
        struct LinkSlot *slot;
        s32 *role;
        for (i = 0, zero2 = 0, role = &p->role, slot = (struct LinkSlot *)p->slotData; i <= 1;
             slot++, i++) {
            slot->prev = slot->cur;
            if (*role < 0)
                slot->cur = zero2;
            else
                slot->cur = peerStatus(i);
            if (slot->prev != slot->cur)
                ((struct LinkPanel *)(i * sizeof(struct LinkSlot) + (u32)p))->slotData[2] = zero2;
        }
    }
    SlotMenu_UpdateLastEntry(p);
    LinkPanel_UpdateAnimations(p);
    PaletteFade_CycleMenuColors(p->palette);
    LinkPanel_UpdateStatusMessage(p);
    p->oldNumber = p->peerIndex;
    if (p->timer <= 0x3b) {
        p->timer++;
    } else if (!unavailable()) {
        if (gUnk_03005260[1] & 2) {
            SetUpdate(p, SlotMenu_UpdateExitTransition, 1);
            Sound_PlaySong(0xde);
        } else {
            p->flags &= ~1;
        }
    } else {
        p->flags |= 1;
    }
}
