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
void ScenePanelRig_TickApproachSubject(struct ScenePanelRig *rig)
{
    s32 targetAngle;
    union {
        s32 distance;
        const s16 *table;
    } work;
    s32 difference;
    s32 distanceDifference;
    u32 angle;
    s32 value, wave;
    register s32 x;
    s32 z;
    if (rig->selectedSubject == 0) {
        targetAngle = 1920;
        work.distance = 480;
    } else if (rig->selectedSubject == 1) {
        targetAngle = 3968;
        work.distance = 400;
    } else if (rig->selectedSubject == 2) {
        targetAngle = 3200;
        work.distance = 480;
    } else {
        targetAngle = 1152;
        work.distance = 400;
    }
    {
        u16 *anglePointer = &rig->angle;
        u32 current = *anglePointer;
        s32 wrapped = current - 4096;
        difference = (targetAngle - wrapped) & 4095;
        if (difference <= 2047) {
            if (difference <= 47)
                rig->angle = targetAngle;
            else
                rig->angle = (rig->angle + 48) & 4095;
        } else {
            if (difference > 4048)
                rig->angle = targetAngle;
            else
                rig->angle = (rig->angle + 4048) & 4095;
        }
    }
    {
        u32 current = rig->angle;
        angle = current >> 4;
    }
    {
        u16 *distance = &rig->distance;
        s32 previous = *distance;
        distanceDifference = work.distance - previous;
        *distance = (previous * 7 + work.distance) >> 3;
        {
            s32 radius = *distance;
            const s16 *lookup;
            lookup = gUnk_086149C4;
            value = lookup[(angle + 64) & 255] * radius;
            work.table = lookup;
        }
    }
    x = scale12(value);
    wave = work.table[(rig->motionFlag + 64) & 255];
    value = (work.table[(angle + 64) & 255] << 5) * wave;
    {
        register s32 modulation asm("r2") = scale24(value);
        u32 position;
        asm("" : "+r"(modulation));
        position = (u16)rig->origin.x + x;
        position += modulation;
        rig->position.x = position;
    }
    {
        u32 radius;
        u16 *address;
        address = &rig->distance;
        radius = *address;
        value = work.table[angle & 255] * radius;
    }
    z = scale12(value);
    wave = work.table[(rig->motionFlag + 64) & 255];
    angle &= 255;
    {
        const s16 *address = (void *)(angle * 2 + (u32)work.table);
        value = (*address << 5) * wave;
    }
    {
        register s32 modulation asm("r1") = scale24(value);
        u32 position;
        asm("" : "+r"(modulation));
        position = (u16)rig->origin.z + z;
        position += modulation;
        rig->position.z = position;
    }
    if (difference == 0) {
        s32 absolute = distanceDifference;
        if (absolute < 0)
            absolute = -absolute;
        if (absolute <= 31) {
            Sound_PlaySong(244);
            ScenePanelRig_SetState(rig, 5);
        }
    }
}
