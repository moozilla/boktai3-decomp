#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Entry {
    u8 bytes[4], a, b, flag, kind;
    u32 timer;
};
struct Packet {
    u8 pad, type;
    u8 pad2[2];
    s32 word;
};
struct P {
    u8 pad[0x20];
    u32 flags;
    u8 pad1[7];
    u8 sent;
    u8 pad2[3];
    u8 mode;
    u8 scores[4];
    u8 pad3[8];
    struct Entry entries[2];
    struct Packet packet;
};
struct P *WirelessExchange_Get(void);
void WirelessExchange_ApplyReceivedCounts(struct P *);
int Wireless_IsSessionActive(void);
int Wireless_QueueNiPacket(struct Packet *);
void WirelessGameItems_QueueIncomingItem(int, int, int);
void WirelessExchange_OnPacketReceived(struct Packet *p)
{
    struct P *s = WirelessExchange_Get();
    struct Entry *e;
    int i;
    register int shift asm("r5");
    int value;
    u32 mask;
    if (!s)
        return;
    e = &s->entries[1];
    switch (p->type) {
        case 0:
            e->a = p->word;
            e->b = (u32)p->word >> 8;
            shift = 16;
            value = p->word;
            i = 0;
            do {
                e->bytes[i] = (value >> shift) & 15;
                i++;
                shift += 4;
            } while (i <= 3);
            WirelessExchange_ApplyReceivedCounts(s);
            break;
        case 1:
            mask = 2;
            goto flags;
        case 2:
            shift = 0;
            {
                register u32 v asm("r0") = *(volatile u16 *)((u8 *)p + 6);
                v &= 1;
                e->flag = v;
            }
            value = p->word;
            i = 0;
            do {
                e->bytes[i] = (value >> shift) & 15;
                i++;
                shift += 4;
            } while (i <= 3);
            mask = 8;
            goto flags;
        case 3:
            mask = 32;
            goto flags;
        case 4:
            e->timer = *(u16 *)&p->word;
            {
                register u32 v asm("r0") = (*(volatile u16 *)((u8 *)p + 6)) & 3;
                u32 z;
                e->kind = v;
                z = 0;
                if (v == 1)
                    e->b = z;
                else if (v == 2)
                    e->a = z;
            }
            mask = 128;
            goto flags;
        case 5:
            if (!Wireless_IsSessionActive())
                s->mode = p->word & 3;
            mask = 512;
        flags:
            s->flags |= mask;
            break;
    }
}
void WirelessExchange_OnPacketSent(struct Packet *p)
{
    struct P *s = WirelessExchange_Get();
    u32 mask;
    if (s) {
        switch (p->type) {
            case 1:
                mask = 1;
                goto flags;
            case 2:
                mask = 4;
                goto flags;
            case 3:
                mask = 16;
                goto flags;
            case 4:
                mask = 64;
                goto flags;
            case 5:
                mask = 256;
            flags:
                s->flags |= mask;
                break;
            case 0:
            default:
                break;
        }
        s->sent = 1;
    }
}
void WirelessExchange_OnSendFailure(void)
{
    struct P *s = WirelessExchange_Get();
    if (s && s->packet.type)
        Wireless_QueueNiPacket(&s->packet);
}
void WirelessExchange_OnReceiveFailure(void)
{
    WirelessExchange_Get();
}
void WirelessExchange_ApplyReceivedCounts(struct P *s)
{
    struct Entry *a = &s->entries[0];
    u8 *q = s->entries[1].bytes;
    int i;
    u32 z;
    if (a->flag) {
        u8 *d;
        u8 *t;
        i = 0;
        z = 0;
        t = q;
        d = q - 16;
        do {
            *d += *t;
            WirelessGameItems_QueueIncomingItem(i, *t, 1);
            *t = z;
            t++;
            d++;
            i++;
        } while (i <= 3);
    } else {
        i = 0;
        z = 0;
        do {
            u8 *p = q + i;
            WirelessGameItems_QueueIncomingItem(i, *p, 0);
            *p = z;
            i++;
        } while (i <= 3);
    }
}
