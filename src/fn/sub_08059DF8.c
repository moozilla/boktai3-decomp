#include "scene_panel_rig.h"
static inline s32 scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline s32 scale24(s32 value)
{
    return value >= 0 ? value >> 24 : -((-value) >> 24);
}
static inline u32 readWord(const u16 *table, u32 index)
{
    return table[index];
}
#define SCALE12(out, n)                                                                            \
    do {                                                                                           \
        (out) = scale12(n);                                                                        \
    } while (0)
void ScenePanelRig_TickWandering(struct ScenePanelRig *rig)
{
    u32 angle;
    union {
        u32 turnStep;
        const u16 *randomTable;
    } work;
    s32 value;
    s32 wave;
    s32 x;
    s32 z;
    u16 *timer;
    work.turnStep = (u16)rig->turnStep;
    if (rig->turnStep < rig->turnRate)
        rig->turnStep = work.turnStep + 1;
    else if (rig->turnStep > rig->turnRate)
        rig->turnStep = work.turnStep - 1;
    timer = &rig->turnDuration;
    if (--*timer == 0) {

        u32 *cursor;
        u32 mask;
        u16 index;
        s32 step;
        work.randomTable = gRandomTable;
        cursor = &gRandomIndex;
        index = *cursor + 1;
        mask = 1023;
        index &= mask;
        step = (readWord(work.randomTable, index) & 7) + 12;
        rig->turnRate = step;
        *cursor = index = (index + 1) & mask;
        if (readWord(work.randomTable, index) & 1)
            rig->turnRate = -step;
        *cursor = (*cursor + 1) & mask;
        *timer = (readWord(work.randomTable, *cursor) & 63) + 32;
    }
    rig->angle = (rig->angle + 4095 + (u16)rig->turnStep) & 4095;
    angle = rig->angle >> 4;
    timer = &rig->verticalStep;
    if (--*timer == 0) {
        const u16 *table = gRandomTable;
        u32 *cursor = &gRandomIndex;
        u32 mask;
        u16 index;
        index = *cursor + 1;
        mask = 1023;
        index &= mask;
        rig->speed = *(const u8 *)((u32)table + index * 2) + 192;
        *cursor = index = (index + 1) & mask;
        *timer = (readWord(table, index) & 63) + 64;
    }
    {
        u16 *distance = &rig->distance;
        s32 previous = *distance;
        *distance = (previous * 15 + rig->speed) >> 4;
        {
            s32 radius = *distance;
            value = gUnk_086149C4[(angle + 64) & 255] * radius;
        }
    }
    SCALE12(x, value);
    wave = gUnk_086149C4[rig->motionFlag];
    value = (gUnk_086149C4[(angle + 64) & 255] << 5) * wave;
    rig->position.x = rig->origin.x + x + scale24(value);
    {
        u32 radius = rig->distance;
        value = gUnk_086149C4[angle] * radius;
    }
    SCALE12(z, value);
    wave = gUnk_086149C4[rig->motionFlag];
    value = (gUnk_086149C4[angle & 255] << 5) * wave;
    rig->position.z = rig->origin.z + z + scale24(value);
    if (rig->selectionTimer <= 2) {
        rig->selectionTimer = 600;
        rig->selectedSubject = ScenePanelRig_SelectActiveSubject(rig);
        if (rig->selectedSubject >= 0)
            ScenePanelRig_SetState(rig, 4);
    } else
        rig->selectionTimer -= 2;
}
