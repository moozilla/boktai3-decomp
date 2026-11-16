#include "scene_panel_rig.h"

static inline s32 scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline s32 scale24(s32 value)
{
    return value >= 0 ? value >> 24 : -((-value) >> 24);
}

// Turns the rig toward the selected subject's angle and eases its distance,
// then places it on that circle with a small sway; arriving plays a sound and
// moves on to state 5.
void ScenePanelRig_TickApproachSubject(struct ScenePanelRig *rig)
{
    s32 targetAngle;
    // The target distance and then the sine table share one register.
    union {
        s32 distance;
        const s16 *table;
    } work;
    s32 difference;
    s32 distanceDifference;
    u32 angle;
    s32 value, wave, sway;
    void *address;
    s32 x, z;

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
    if (value >= 0)
        wave = value >> 24;
    else
        wave = -((-value) >> 24);
    {
        u32 position;
        position = (u16)rig->origin.x + x;
        position += wave;
        rig->position.x = position;
    }
    {
        u32 radius;
        address = &rig->distance;
        radius = *(u16 *)address;
        value = work.table[angle & 255] * radius;
    }
    z = scale12(value);
    sway = work.table[(rig->motionFlag + 64) & 255];
    angle &= 255;
    // `address` is reused here; sharing it with the distance read keeps the
    // table's register preference away from the sway result.
    address = (void *)(angle * 2 + (u32)work.table);
    value = (*(s16 *)address << 5) * sway;
    {
        s32 modulation = scale24(value);
        u32 position;
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
