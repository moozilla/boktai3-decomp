// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
#define ScenePanelRig_TickApproachSubject sub_0805A044
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
    s32 targetDistance;
    s32 difference;
    s32 distanceDifference;
    u32 angle;
    s32 value, wave;
    u32 originOffset;
    register s32 x asm("r8");
    s32 z;
    const s16 *table;
    if (rig->selectedSubject == 0) {
        targetAngle = 1920;
        targetDistance = 480;
    } else if (rig->selectedSubject == 1) {
        targetAngle = 3968;
        targetDistance = 400;
    } else if (rig->selectedSubject == 2) {
        targetAngle = 3200;
        targetDistance = 480;
    } else {
        targetAngle = 1152;
        targetDistance = 400;
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
        distanceDifference = targetDistance - previous;
        *distance = (previous * 7 + targetDistance) >> 3;
        {
            s32 radius = *distance;
            const s16 *lookup;
            lookup = gUnk_086149C4;
            value = lookup[(angle + 64) & 255] * radius;
            table = lookup;
        }
    }
    x = scale12(value);
    wave = table[(rig->motionFlag + 64) & 255];
    value = (table[(angle + 64) & 255] << 5) * wave;
    {
        register s32 modulation asm("r2") = scale24(value);
        u32 position;
        asm("" : "+r"(modulation));
        originOffset = 0x674;
        position = *(u16 *)((u8 *)rig + originOffset) + x;
        position += modulation;
        rig->position.x = position;
    }
    {
        u32 radius;
        u16 *address;
        originOffset += 0x14;
        address = (void *)((u8 *)rig + originOffset);
        radius = *address;
        value = table[angle & 255] * radius;
    }
    z = scale12(value);
    asm("" ::"r"(originOffset));
    wave = table[(rig->motionFlag + 64) & 255];
    angle &= 255;
    {
        const s16 *address = (void *)(angle * 2 + (u32)table);
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
