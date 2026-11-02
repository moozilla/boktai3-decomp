#include "vanargand_damage_part.h"

#define Vanargand_ApplyPartDamage sub_081A7204

void SceneAnimation_SelectSequence(void *, s32, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);

void Vanargand_ApplyPartDamage(struct VanargandDamagePart *part)
{
    u32 damage;
    u32 flags;
    damage = part->damage = part->pendingDamage * 2;
    part->contactFlags = part->pendingFlags;
    flags = (part->pendingFlags >> 6) & 1;
    part->pendingDamage = 0;
    part->pendingFlags = 0;
    if ((s32)damage > 0 || flags) {
        u32 total = part->accumulatedDamage + damage;
        s32 mode;
        part->accumulatedDamage = total;
        mode = *part->mode;
        if (mode > 2)
            mode = 2;
        if ((u16)total >= part->thresholds[mode]) {
            part->flags |= 4;
            SceneAnimation_SelectSequence(part, 2, 0, 0);
            part->accumulatedDamage = 0;
            part->breaks += 1;
            Sound_PlaySong(0x579);
        } else {
            if (!flags)
                SceneAnimation_SelectSequence(part, 1, 0, 2);
            part->flags |= 2;
            Sound_PlaySong(318);
        }
        part->flashTimer = 6;
        AnimData_SelectPaletteBank((u8 *)part + 0x2C, 402);
    }
}
