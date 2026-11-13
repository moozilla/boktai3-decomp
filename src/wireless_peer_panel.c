#include "wireless_peer_panel.h"
#include "input.h"
#include "display_sequence_inline.h"

static inline void WirelessPeerPanel_StepSprite(struct ContactSwarmSprite *sprite,
                                                struct DisplaySequenceState *a)
{
    const u16 *frame;
    frame = a->commands + a->index;
    sprite->frame = *frame >> 6;

    // Combine the animation and frame orientation flags for the sprite.
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }

    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }

    if (++a->elapsed >= a->duration) {
        DisplaySequence_SelectNext(a);
        frame = a->commands + a->index;
        DisplaySequence_UpdateWeight(a, frame);
        if (!a->duration)
            a->duration = 1;
    }
}

void WirelessPeerMenu_InitBackground(struct WirelessPeerMenu *menu, int layer)
{
    void *resource = ScriptProc_Start(0xC091, 0xCF15);
    int zero;
    menu->background = resource;
    zero = 0;
    Bg_SetupLayers(1, 0, resource, 0, zero, 1, &layer);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    {
        struct PaletteResource *palette = ScriptProc_Start(0x92B3, 0x26BB);
        CpuSet(palette->banks, gBgPaletteBuffer, 256);
    }
}

int WirelessPeerPanel_Init(struct WirelessPeerMenu *owner, struct WirelessPeerPanel *panel,
                           int kind)
{
    struct WirelessPeerPanel *panelState = panel;
    int type = kind;
    void *resource;
    void *glyph;
    const struct WirelessPeerStyle *style;
    struct ContactSwarmSprite *sprite;
    struct ContactSwarmSprite *initialSprite;
    struct DisplaySequenceState animation;

    resource = ScriptProc_Start(0x922E, 0x4BBF);
    glyph = panelState->graphics;
    {
        const struct WirelessPeerStyle *table = gUnk_0824DE64;
        {
            int index = type;
            {
                u32 offset = index << 1;
                offset += type;
                offset <<= 2;
                style = (const struct WirelessPeerStyle *)(offset + (u32)table);
            }
        }
    }
    AnimData_Load(glyph, style->resource);
    {
        int tile = style->paletteBank;
        int base = 503;
        AnimData_SelectPaletteBank(glyph, tile + base);
    }
    initialSprite = &panelState->sprite;
    Sprite_Init(initialSprite, glyph, style->priority | 48);
    Anim_RestartSequence(&animation, resource, style->animation, 0, 0);
    sprite = initialSprite;
    WirelessPeerPanel_StepSprite(sprite, &animation);
    {
        int one = 1;
        struct WirelessPeerPanel *write;
        const struct WirelessPeerStyle *table;
        int kind;
        u32 offset;
        const struct WirelessPeerStyle *entry;
        write = panelState;
        write->sprite.priority = one;
        table = gUnk_0824DE64;
        kind = type;
        offset = kind << 1;
        offset += type;
        offset <<= 2;
        entry = (const struct WirelessPeerStyle *)(offset + (u32)table);
        write->sprite.position.x = entry->x;
        write->sprite.position.y = entry->y;
    }
    {
        int zero = 0;
        struct SceneInputWidget *drawing;
        int count;
        drawing = &panelState->digits[0];
        count = 4;
        do {
            Widget_InitLinked(drawing, &owner->digitResource, 0, 17, 1, zero, 60, (void *)zero);
            drawing++;
        } while (--count >= 0);
    }
    panelState->active = 0;
    panelState->name[0] = 0;
    panelState->confirmationName[0] = 0;
    return 0;
}

void WirelessPeerPanel_Destroy(void *owner, struct WirelessPeerPanel *panel)
{
    struct SceneInputWidget *drawing;
    int i;
    Sprite_Remove(&panel->sprite);
    drawing = &panel->digits[0];
    i = 4;
    do {
        Anim_Remove(drawing);
        drawing++;
    } while (--i >= 0);
}

u32 WirelessPeerPanel_SetAvailable(void *owner, struct WirelessPeerPanel *panel)
{
    panel->sprite.flags &= ~1;
}

u32 WirelessPeerPanel_SetUnavailable(void *owner, struct WirelessPeerPanel *panel)
{
    panel->sprite.flags |= 1;
}

int WirelessPeerPanel_SetNameFromPacket(void *ownerArg, struct WirelessPeerPanel *panel,
                                        int peerArg, int playerArg, const u8 *packetArg)
{
    struct WirelessPeerMenu *owner = ownerArg;
    struct WirelessPeerPanel *panelState = panel;
    int peer = peerArg;
    int player = playerArg;
    const u8 *packet = packetArg;
    void *name;
    u32 zero;
    u8 *scratch;
    u8 *destination;

    name = WirelessExchange_GetPeerEntry(peer);
    panelState->flags = 0;
    zero = 0;
    scratch = owner->scratch;
    CpuSet(&zero, scratch, 0x05000001);
    packet += 4 + player * 10;
    Memory_Copy(scratch, packet, 16);
    destination = panelState->name;
    WirelessText_DecodeString(scratch, destination, 8);
    Memory_Copy(name, destination, 16);
    {
        int window = owner->windows[peer];
        peer += 3;
        TextWindow_Refresh(window, peer);
    }
}

int WirelessPeerPanel_ClearName(struct WirelessPeerMenu *owner, struct WirelessPeerPanel *panel,
                                int peer)
{
    void *name = WirelessExchange_GetPeerEntry(peer);
    u8 *destination;
    int i;
    int zero;

    panel->flags = 0;
    zero = 0;
    i = 15;
    destination = &panel->name[15];
    do {
        *destination-- = zero;
    } while (--i >= 0);
    Memory_Copy(name, panel->name, 16);
    {
        int id = owner->windows[peer];
        TextWindow_Refresh(id, peer + 3);
    }
}

int WirelessPeerMenu_GetFirstConnectedPlayer(const u8 *handshake)
{
    int slots = Wireless_GetConnectedSlotMask();
    int i = 0;
    int result;
    do {
        if ((slots >> i) & 1) {
            result = i + 1;
            goto done;
        }
    } while (++i <= 3);
    result = 0;
done:
    return result;
}

void WirelessPeerPanel_Update(struct WirelessPeerMenu *menu, struct WirelessPeerPanel *panel,
                              int index)
{
    const u8 *status = Wireless_GetPeerHandshakeData();
    int peer;
    if (!index) {
        int value = Wireless_GetLocalSessionId();
        Anim_DrawDecimalDigits(panel->digits, &menu->digitResource, value, 5, 0, 24, 70, 8, 0);
        goto available;
    }
    if (Wireless_GetConnectedSlotMask()) {
    available:
        WirelessPeerPanel_SetAvailable(menu, panel);
    } else
        WirelessPeerPanel_SetUnavailable(menu, panel);
    if (status) {
        peer = WirelessPeerMenu_GetFirstConnectedPlayer(status);
        if (!index) {
            if (!panel->active) {
                void *name = WirelessExchange_GetPeerEntry(0);
                u32 zero = 0;
                CpuSet(&zero, menu->scratch, 0x05000001);
                Memory_Copy(name, gGameState->playerName, 16);
                Memory_Copy(panel->name, gGameState->playerName, 16);
                TextWindow_Refresh(menu->windows[0], 3);
                panel->active = 1;
            }
        } else if (peer) {
            if (!panel->active) {
                WirelessPeerPanel_SetNameFromPacket(menu, panel, index, peer, status);
                panel->active = 1;
            }
        } else if (panel->active) {
            WirelessPeerPanel_ClearName(menu, panel, index);
            panel->active = 0;
        }
    }
}

void WirelessPeerMenu_UpdateCancelInput(struct WirelessPeerMenu *menu)
{
    if (menu->entered)
        menu->entered = 0;
    if (menu->timer > 15) {
        int busy = Wireless_IsSessionActive();
        if (busy) {
            if (gPadState[1] & 2) {
                Wireless_RequestAdvanceConnection();
                {
                    u32 state = 7;
                    u32 zero = 0;
                    menu->state = state;
                    menu->entered = 1;
                    menu->timer = zero;
                }
            }
        } else {
            if (gPadState[1] & 0x200) {
                Wireless_RequestAdvanceConnection();
                menu->state = 7;
                menu->entered = 1;
                menu->timer = busy;
            }
        }
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdateDismissibleNotice(struct WirelessPeerMenu *menu)
{
    if (menu->entered) {
        menu->entered = 0;
        TextWindow_Refresh(menu->windows[2], 2);
        Sound_PlaySong(402);
    }
    if (menu->timer > 35) {
        if (gPadState[1] & 3) {
            Wireless_RequestAdvanceConnection();
            {
                u32 state = 7;
                u32 zero = 0;
                menu->state = state;
                menu->entered = 1;
                menu->timer = zero;
            }
        }
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdatePeerConfirmation(struct WirelessPeerMenu *menu)
{
    u8 alias;
    u8 connected = Wireless_GetConnectedSlotMask();
    struct WirelessPeerPanel *remote;
    int i;
    if (menu->entered) {
        menu->entered = 0;
        TextWindow_Refresh(menu->windows[2], 7);
        menu->previousSlots = 255;
    }
    if (connected != menu->previousSlots) {
        remote = &menu->panels[1];
        if (connected && (connected == gRfuLinkStatus->connSlotFlag)) {
            if (Wireless_GetPeerHandshakeData()) {
                u8 *name;
                void *draw;
                int bit;
                i = 0;
                alias = gRfuLinkStatus->connSlotFlag;
                {
                    int tested = alias & 1;
                    bit = tested;
                }
                name = menu->panels[1].confirmationName;
                draw = menu->panels[1].digits;
                if (!bit) {
                    struct RfuLinkStatus **links = &gRfuLinkStatus;
                    do {
                        i++;
                        if (i > 3) {
                            break;
                        }
                    } while (!(((*links)->connSlotFlag >> i) & 1));
                }
                remote = &menu->panels[1];
                Memory_Clear(name, 24);
                WirelessText_DecodeString(gRfuLinkStatus->partner[i].gname, name, 11);
                {
                    u8 *packet = gRfuLinkStatus->partner[i].gname;
                    int value = packet[11] | (packet[12] << 8);
                    Anim_DrawDecimalDigits(draw, &menu->digitResource, value, 5, 0, 136, 70, 8, 0);
                }
                {
                    u32 mask = ~1;
                    /* Keep the panel-relative digit field while advancing one widget. */
                    struct WirelessPeerPanel *row = remote;
                    i = 0;
                    do {
                        row->digits[0].flags &= mask;
                        row = (struct WirelessPeerPanel *)((u8 *)row +
                                                           sizeof(struct SceneInputWidget));
                    } while (++i < 5);
                }
                TextWindow_SetString(menu->windows[2], 2, remote->confirmationName);
                Sound_PlaySong(495);
            }
            TextWindow_Refresh(menu->windows[2], 8);
        } else {
            TextWindow_Refresh(menu->windows[2], 7);
            {
                u32 mask = 1;
                /* Keep the panel-relative digit field while advancing one widget. */
                struct WirelessPeerPanel *row = remote;
                i = 0;
                do {
                    row->digits[0].flags |= mask;
                    row = (struct WirelessPeerPanel *)((u8 *)row + sizeof(struct SceneInputWidget));
                } while (++i < 5);
            }
        }
        menu->previousSlots = connected;
    }
    if ((gPadState[1] & 1) && connected) {
        Wireless_RequestAdvanceConnection();
    }
    if (gPadState[1] & 2) {
        if (connected) {
            Wireless_RequestDisconnectPeer();
        } else {
            Wireless_RequestAdvanceConnection();
            menu->state = 7;
            menu->entered = 1;
            menu->timer = connected;
        }
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdateStatusNotice(struct WirelessPeerMenu *menu)
{
    if (menu->entered) {
        menu->entered = 0;
        TextWindow_Refresh(menu->windows[2], 9);
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdatePeerUnavailable(struct WirelessPeerMenu *menu)
{
    if (menu->entered) {
        struct WirelessPeerPanel *remote;
        int i;
        u32 mask;
        struct WirelessPeerPanel *row;
        menu->entered = 0;
        remote = &menu->panels[1];
        TextWindow_Refresh(menu->windows[2], 6);
        mask = 1;
        row = remote;
        i = 0;
        do {
            row->digits[0].flags |= mask;
            row = (struct WirelessPeerPanel *)((u8 *)row + sizeof(struct SceneInputWidget));
        } while (++i < 5);
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdateStatusElevenNotice(struct WirelessPeerMenu *menu)
{
    if (menu->entered != 0) {
        menu->entered = 0;
        TextWindow_Refresh(menu->windows[2], 0);
        Sound_PlaySong(0x192);
    }
    menu->timer += 1;
}

void WirelessPeerMenu_UpdateStatusTwentyEightNotice(struct WirelessPeerMenu *menu)
{
    if (menu->entered != 0) {
        menu->entered = 0;
        TextWindow_Refresh(menu->windows[2], 1);
        Sound_PlaySong(0x192);
    }
    menu->timer += 1;
}

void WirelessPeerMenu_UpdateExit(struct WirelessPeerMenu *menu)
{
    if (menu->entered != 0) {
        menu->entered = 0;
        Sound_PlaySong(0xde);
        Wireless_RequestResetConnection();
    }
    if (menu->timer == 4) {
        ScreenTransition_SendCommand(3, 4, 4, 4, menu->timer, 0xFFFF, 0);
    }
    if (menu->timer == 0x18) {
        if (menu->completionProcedure != 0)
            Script_Call(menu->completionProcedure, 0);
    }
    menu->timer++;
}

void WirelessPeerMenu_UpdateConnectionState(struct WirelessPeerMenu *menu)
{
    s32 r = Wireless_GetOperation();
    u32 v;
    if (menu->state != 7) {
        if (r != menu->operation) {
            switch (r) {
                case 27: {
                    u32 one = 1;
                    u32 z = 0;
                    menu->state = one;
                    menu->entered = one;
                    menu->timer = z;
                    break;
                }
                case 7:
                    v = 2;
                    goto s1;
                case 9:
                    v = 3;
                    goto s1;
                case 11:
                    v = 4;
                    goto s1;
                case 28:
                    v = 5;
                    goto s1;
                case 29:
                    v = 6;
                s1:
                    menu->state = v;
                    menu->entered = 1;
                    menu->timer = 0;
                    break;
                default:
                    break;
            }
        }
        menu->operation = r;
    }
}
