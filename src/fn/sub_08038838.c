#include "global.h"

// Tentative link result layout: four scores are capped and packed as nibbles.
struct Entry {
    u8 bytes[4], a, b, flag, kind;
    u32 timer;
};

struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[14];
    u8 scores[4];
    struct Entry entry;
};

void WirelessGame_AddWeightedScore(int, int);
int WirelessExchange_SendPacket(struct P *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void WirelessGameMenu_ConsumeSelectedItem(struct P *);
void WirelessGameMenu_ConsumeSelectedEquipment(struct P *);
void WirelessExchange_ApplyReceivedCounts(struct P *);
void WirelessGameMenu_SignalStatusPanel(struct P *);

static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void WirelessGameMenu_UpdateSelectionExchange(struct P *p)
{
    u32 data;
    if (p->entered) {
        struct Entry *e;
        u32 packed;
        int total;
        int i;
        int shift;
        u8 *q;
        p->entered = 0;
        do {
            data = 0;
            e = &p->entry;
            packed = 0;
            total = 0;
            i = 0;
            shift = 0;
            q = e->bytes;
        } while (0);
        do {
            int value;
            u32 flag = e->flag;
            if (flag) {
                value = *q;
            } else {
                int old = *q;
                u8 *score;
                int n;
                score = p->scores;
                score += i;
                n = *score;
                value = old + n;
                total += n;
                *score = (float)flag;
            }
            e->bytes[i] = 0;
            if (value > 15) {
                value = 15;
            }
            packed |= value << shift;
            q++;
            i++;
            shift += 4;
        } while (i <= 3);
        if (total > 0) {
            WirelessGame_AddWeightedScore(3, 1);
        }
        data |= packed;
        WirelessExchange_SendPacket(p, 2, &data, 1);
        WirelessGameMenu_SetStatusPanelState(p, 1);
        WirelessGameMenu_ConsumeSelectedItem(p);
        WirelessGameMenu_ConsumeSelectedEquipment(p);
    }
    if (test(&p->flags, 12) == 12) {
        u32 five;
        u8 *state;
        u32 zero;
        WirelessExchange_ApplyReceivedCounts(p);
        {
            u32 mask = ~12;
            p->flags &= mask;
        }
        five = 5;
        state = &p->state;
        zero = 0;
        *state = five;
        p->entered = 1;
        p->timer = zero;
        WirelessGameMenu_SignalStatusPanel(p);
    } else {
        p->timer++;
    }
}
