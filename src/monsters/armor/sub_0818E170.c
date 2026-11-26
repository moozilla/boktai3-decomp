#include "monsters/armor/armor_part_queue.h"
void Armor_InitPartOrbitTimer(struct ArmorLaunchPart *);
void Armor_UpdatePartBobbing(void *);

static inline u8 reset(struct ArmorLaunchPart *part)
{
    s32 one = 1;
    u8 old = part->flags;
    if (old & one) {
        s32 value = ~1;
        value &= old;
        part->flags = value;
        part->motionPhase = 0;
        return 1;
    }
    return 0;
}
void Armor_StartPartOrbitMotion(struct ArmorLaunchPart *part)
{
    if (reset(part))
        part->update = Armor_InitPartOrbitTimer;
    Armor_UpdatePartBobbing(part);
}
