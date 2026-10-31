#include "global.h"
#include "engine_globals.h"
#include "ram.h"

struct ColorPulsePart {
    u32 flags;
    u8 pad4[2];
    u8 mode;
    u8 pad7;
    s8 weight;
    u8 pad9[0x13];
    s16 x, y, z;
    u8 pad22[0xA];
};

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.
struct ColorPulseTarget {
    u8 pad0[0x30];
    s16 x, y, z;
};
struct ColorPulsePoint {
    s16 x, y, z, pad;
};
struct ColorPulse {
    u8 pad0[0x60];
    struct ColorPulseTarget *target;
    s16 posA;
    u8 pad66[2];
    s16 posB;
    u8 pad6A[2];
    u16 counterA;
    u8 pad6E[2];
    u16 counterB;
    u8 pad72[0xA8 - 0x72];
    struct ColorPulsePart entries[4];
    u8 pad158[0x468 - 0x158];
    struct ColorPulsePoint saved[4];
    struct ColorPulsePoint goal;
    u8 pad490[0xB6E - 0x490];
    u8 phase;
    u8 padB6F[0xB84 - 0xB6F];
    struct ColorPulsePoint origin;
    u8 padB8C[0xCE0 - 0xB8C];
    void (*callbackCE0)(struct ColorPulse *);
    u8 padCE4[0xCF4 - 0xCE4];
    s32 delay;
    u8 padCF8[0xD80 - 0xCF8];
    u32 flagsD80;
};
extern s16 gUnk_086149C4[];
s32 Div(s32, s32);
void PhaseObject_BeginEntryMotion(struct ColorPulse *);

static inline u32 PhaseObject_RandomHeading(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 mask = 255;
    u32 value;
    gRandomIndex = index;
    index *= 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    return value & mask;
}

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.
void PhaseObject_UpdateEntryScatter(struct ColorPulse *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        struct ColorPulse *entry;
        p->posA = -p->posA;
        p->posB = -p->posB;
        entry = (void *)((u8 *)p + i * 0x2c);
        entry->entries[0].x += p->posA;
        entry->entries[0].z -= p->posB;
        entry->entries[0].weight -= 2;
        if (entry->entries[0].weight <= 1)
            entry->entries[0].weight = 1;
    }
    if (!(p->flagsD80 & 1)) {
        p->counterA++;
        p->counterB++;
        p->posA = p->counterA;
        p->posB = p->counterB;
        if ((s16)p->counterA > 15) {
            u32 sel;

            p->posA = p->entries[0].x - p->origin.x;
            p->posB = p->entries[0].z - p->origin.z;
            sel = PhaseObject_RandomHeading();
            p->origin.x = p->target->x + Div(gUnk_086149C4[(sel + 64) & 255] * 200, 4096);
            p->origin.y = p->target->y + 350;
            p->origin.z = p->target->z + Div(gUnk_086149C4[sel & 255] * 200, 4096);
            for (i = 0; i < 4; i++) {
                struct ColorPulse *entry;

                p->entries[i].flags |= 1;
                entry = (void *)((u8 *)p + i * 0x2c);
                entry->entries[0].x = p->saved[i].x = p->origin.x + p->posA;
                entry->entries[0].z = p->saved[i].z = p->origin.z + p->posB;
                p->goal.x = p->origin.x;
                p->goal.y = p->origin.y;
                p->goal.z = p->origin.z;
                p->posA = -p->posA;
                p->posB = -p->posB;
                entry->entries[0].mode = 0x80;
            }
            p->phase = 0x80;
            p->delay = 30;
            p->callbackCE0 = PhaseObject_BeginEntryMotion;
        }
    }
}
