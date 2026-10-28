// MATCHING_CONCESSION: register-pinning
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
void ScenePanelRig_TickWandering(struct ScenePanelRig *rig)
{
    u32 angle;
    s32 value;
    s32 wave;
    register s32 x asm("r3");
    s32 z;
    u16 *timer;
    if (rig->turnStep < rig->turnRate)
        rig->turnStep++;
    else if (rig->turnStep > rig->turnRate)
        rig->turnStep--;
    timer = &rig->turnDuration;
    if (--*timer == 0) {
        register const u16 *table asm("r3") = gRandomTable;
        u32 *cursor = &gRandomIndex;
        u32 mask;
        register u32 index asm("r1");
        s32 step;
        index = *cursor + 1;
        mask = 1023;
        index &= mask;
        step = (readWord(table, index) & 7) + 12;
        rig->turnRate = step;
        *cursor = index = (index + 1) & mask;
        if (readWord(table, index) & 1)
            rig->turnRate = -step;
        *cursor = (*cursor + 1) & mask;
        *timer = (readWord(table, *cursor) & 63) + 32;
    }
    rig->angle = (rig->angle + 4095 + (u16)rig->turnStep) & 4095;
    angle = rig->angle >> 4;
    timer = &rig->verticalStep;
    if (--*timer == 0) {
        const u16 *table = gRandomTable;
        u32 *cursor = &gRandomIndex;
        u32 mask;
        register u32 index asm("r0");
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
    value = scale12(value);
    asm("" : "+r"(value));
    x = value;
    wave = gUnk_086149C4[rig->motionFlag];
    value = (gUnk_086149C4[(angle + 64) & 255] << 5) * wave;
    rig->position.x = rig->origin.x + x + scale24(value);
    {
        u32 radius = rig->distance;
        value = gUnk_086149C4[angle] * radius;
    }
    value = scale12(value);
    z = value;
    asm("" : "+r"(angle));
    wave = gUnk_086149C4[rig->motionFlag];
    value = (gUnk_086149C4[angle] << 5) * wave;
    rig->position.z = rig->origin.z + z + scale24(value);
    if (rig->selectionTimer <= 2) {
        rig->selectionTimer = 600;
        rig->selectedSubject = ScenePanelRig_SelectActiveSubject(rig);
        if (rig->selectedSubject >= 0)
            ScenePanelRig_SetState(rig, 4);
    } else
        rig->selectionTimer -= 2;
}
