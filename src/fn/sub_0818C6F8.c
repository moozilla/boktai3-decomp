#include "global.h"
#include "engine.h"
#include "ram.h"
#include "scene_vec.h"

struct ArmorContact {
    u8 pad0[0x6];
    u16 flags;
    u8 pad8[0x4];
    u16 pos[3];
    u8 pad12[0x22 - 0x12];
    u16 force;
    u8 pad24[0x34 - 0x24];
    u32 contactFlags;
    u8 pad38[0x3E - 0x38];
    u16 damage;
    u8 pad40[0x42 - 0x40];
    u8 direction;
    u8 pad43[0x44 - 0x43];
    u16 responseValue;
};
struct ArmorBody {
    u8 pad0[0x5BE];
    u16 palette;
};
struct ArmorPart {
    u8 pad0[0xC];
    void *animation;
    u8 pad10[0x52 - 0x10];
    u16 field52;
    u8 pad54[0x6A - 0x54];
    u16 force;
    u8 pad6C[0xA6 - 0x6C];
    u16 fieldA6;
    u8 padA8[0xE0 - 0xA8];
    u16 disabled;
    u8 padE2[0x100 - 0xE2];
    s16 velocityX;
    u8 pad102[2];
    s16 velocityZ;
    u8 pad106[0x10A - 0x106];
    s16 health;
    u8 pad10C[0x119 - 0x10C];
    u8 flags;
    u8 direction;
    u8 pad11B[0x11E - 0x11B];
    u8 active;
    u8 phase;
    u8 pad120[0x128 - 0x120];
    u16 paletteTimer;
    u8 pad12A[0x15C - 0x12A];
    struct ArmorBody *parent;
    u8 pad160[0x1CC - 0x160];
    void (*paletteUpdate)(struct ArmorPart *);
    void (*callback)(struct ArmorPart *);
    u8 pad1D4[2];
    u16 timer;
};
extern const s16 gUnk_086149C4[];
void Collision_CalculateDamageResponse(void *, void *);
void Armor_UpdatePartPaletteRestore(struct ArmorPart *);
void Armor_DampPartWaveReturn(struct ArmorPart *);
void Armor_UpdatePartLungeDelay(struct ArmorPart *);
void Armor_UpdatePartLunge(struct ArmorPart *);
void SpinningApproach_UpdateSpinRamp(struct ArmorPart *);
void SpinningApproach_UpdateApproach(struct ArmorPart *);
void SpinningApproach_UpdateOwnerRelativeSegment(struct ArmorPart *);
void Armor_DampPartMovement(struct ArmorPart *);
void SpinningApproach_DampReturnMovement(struct ArmorPart *);
void Armor_CapturePartMotionEndpoint(struct ArmorPart *);

static inline u32 Armor_ContactFlags(struct ArmorContact *contact, u32 mask)
{
    return contact->contactFlags & mask;
}
static inline void Armor_SetPartState(struct ArmorPart *part, u8 active, u8 phase)
{
    part->active = active;
    part->phase = phase;
    part->flags |= active;
}
static inline void Armor_ContactDisplacement(struct SceneVec *out, const struct SceneVec *target,
                                             const struct SceneVec *source)
{
    out->x = source->x - target->x;
    out->y = source->y - target->y;
    out->z = source->z - target->z;
}
static inline u32 Armor_GetPhase(struct ArmorPart *part, u8 **field)
{
    u32 value = part->phase;
    *field = &part->phase;
    return value;
}

void Armor_OnPartDamageCollision(struct ArmorContact *source, struct ArmorContact *target,
                                 struct ArmorPart *part)
{
    struct SceneVec displacement;
    s16 *health;
    struct ArmorBody *parent;
    u32 phaseSnapshot;
    u8 *phase;

    parent = part->parent;
    health = &part->health;
    if (*health <= 0)
        return;
    if (Armor_ContactFlags(source, 0x2000))
        return;
    if (part->disabled)
        return;
    Collision_CalculateDamageResponse(source, target);
    target->responseValue = source->responseValue;
    if (part->paletteUpdate == 0 && target->damage != 0) {
        part->paletteTimer = 8;
        part->paletteUpdate = Armor_UpdatePartPaletteRestore;
        AnimData_SelectPaletteBank(part->animation, parent->palette);
        *health -= target->damage;
        if (*health <= 0) {
            Armor_SetPartState(part, 1, 5);
            Sound_PlaySongChecked(0x13E);
            return;
        }
    }

    phaseSnapshot = Armor_GetPhase(part, &phase);
    if ((phaseSnapshot == 2 && part->callback != Armor_DampPartWaveReturn) ||
        part->callback == Armor_UpdatePartLungeDelay || part->callback == Armor_UpdatePartLunge ||
        part->callback == SpinningApproach_UpdateSpinRamp ||
        part->callback == SpinningApproach_UpdateApproach ||
        part->callback == SpinningApproach_UpdateOwnerRelativeSegment) {
        Armor_CapturePartMotionEndpoint(part);
        switch (*phase) {
            case 2:
                part->timer = 0x80;
                part->callback = Armor_DampPartWaveReturn;
                break;
            case 3:
                part->timer = 0x14;
                part->callback = Armor_DampPartMovement;
                break;
            case 4:
                part->timer = 0x64;
                part->callback = SpinningApproach_DampReturnMovement;
                break;
        }
        if (source->flags & 0x100) {
            part->direction = source->direction;
        } else {
            Armor_ContactDisplacement(&displacement, (void *)target->pos, (void *)source->pos);
            if (displacement.x == 0 && displacement.z == 0) {
                u32 *counter = &gRandomIndex;
                *counter = (*counter + 1) & 0x3FF;
                part->direction = ((const u16 *)RANDOM_TABLE_ADDR)[*counter];
            } else {
                part->direction = Math_Atan2(displacement.x, displacement.z);
            }
        }
        part->fieldA6 = 1;
        part->field52 = 1;
        part->force = source->force;
        part->velocityX = gUnk_086149C4[(part->direction + 0x40) & 0xFF] >> 7;
        part->velocityZ = gUnk_086149C4[part->direction] >> 7;
        Sound_PlaySongChecked(0x170);
    } else {
        if (target->damage != 0)
            Sound_PlaySongChecked(0x13E);
        else
            Sound_PlaySongChecked(0x190);
    }
}
