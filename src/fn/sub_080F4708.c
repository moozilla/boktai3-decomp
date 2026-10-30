#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct SwordContact {
    u8 pad0[0x6];
    u16 flags;
    u8 pad8[0x4];
    u16 pos[3];
    u8 pad12[0x22 - 0x12];
    u16 force;
    u8 pad24[0x34 - 0x24];
    u32 contactFlags;
    u8 pad38[0x3e - 0x38];
    u16 damage;
    u8 pad40[0x42 - 0x40];
    u8 direction;
    u8 pad43[0x44 - 0x43];
    u16 responseValue;
};
struct SwordContactOwner {
    u8 unknown0[0xD1A];
    s16 contactHundredths;
    u8 unknownD1C[0x3C];
    u16 palette;
};
struct SwordComponent {
    u8 pad0[0xc];
    u8 *animation;
    u8 pad10[0x52 - 0x10];
    u16 field52;
    u8 pad54[0x6a - 0x54];
    u16 force;
    u8 pad6c[0xa6 - 0x6c];
    u16 fieldA6;
    u8 padA8[0xe0 - 0xa8];
    u16 disabled;
    u8 padE2[0x100 - 0xe2];
    s16 velocityX;
    u8 pad102[2];
    s16 velocityZ;
    u8 pad106[0x10a - 0x106];
    s16 health;
    u8 pad10c[0x119 - 0x10c];
    u8 flags;
    u8 direction;
    u8 pad11b[0x11e - 0x11b];
    u8 active;
    u8 phase;
    u8 pad120[0x128 - 0x120];
    u16 paletteTimer;
    u8 pad12a[0x160 - 0x12a];
    struct SwordContactOwner *owner;
    void (*update)(void);
    void (*paletteUpdate)(void);
    u8 pad16c[0x174 - 0x16c];
    void (*returnUpdate)(void);
    void (*thrustUpdate)(void);
    void (*lungeUpdate)(void);
    u8 pad180[0x184 - 0x180];
    u32 timer;
    u8 unknown188[0xC];
};
extern const s16 gUnk_086149C4[];
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sword_SetTrajectoryFromCurrentPosition(void *);
void Sword_UpdateComponentPaletteRestore(void);
void Sword_DecelerateReturn(void);
void Sword_UpdateThrustDelay(void);
void Sword_UpdateThrustExtension(void);
void Sword_UpdateLungeSpinup(void);
void Sword_UpdateSpinningLunge(void);
void Sword_DampTargetReturn(void);
void ActorPhase_DampMovement(void);
void TargetMotion_AdvanceFollowSegment(void);
void TargetMotion_UpdateFlashingExit(void);

static inline u32 Sword_ContactFlags(struct SwordContact *contact, u32 mask)
{
    return contact->contactFlags & mask;
}
static inline void Sword_SetChildState(struct SwordComponent *child, u8 active, u8 phase)
{
    child->active = active;
    child->phase = phase;
    child->flags |= active;
}
static inline void Sword_ContactDisplacement(struct SceneVec *out, const struct SceneVec *target,
                                             const struct SceneVec *source)
{
    out->x = source->x - target->x;
    out->y = source->y - target->y;
    out->z = source->z - target->z;
}
static inline u32 Sword_GetPhase(struct SwordComponent *child, u8 **field)
{
    u32 value = child->phase;
    *field = &child->phase;
    return value;
}
static inline void Sword_BeginContactReturn(struct SwordComponent *child, u8 *phaseField)
{
    u32 phase = *phaseField;
    if (phase == 2) {
        child->timer = 0x80;
        child->returnUpdate = Sword_DecelerateReturn;
    } else if (phase == 3) {
        child->timer = 0x14;
        child->thrustUpdate = ActorPhase_DampMovement;
    } else if (phase == 4) {
        child->timer = 0x64;
        child->lungeUpdate = Sword_DampTargetReturn;
    }
}
void sub_080F4708(struct SwordContact *source, struct SwordContact *target,
                  struct SwordComponent *child)
{
    struct SceneVec displacement;
    s16 *health;
    struct SwordContactOwner *owner;
    u32 phaseSnapshot;
    u8 *phase;

    owner = child->owner;
    health = &child->health;
    if (*health <= 0)
        return;
    if (Sword_ContactFlags(source, 0x2000))
        return;
    if (child->disabled)
        return;
    Collision_CalculateDamageResponse(source, target);
    target->responseValue = source->responseValue;
    if (child->paletteUpdate == 0 && target->damage != 0) {
        child->paletteTimer = 8;
        child->paletteUpdate = Sword_UpdateComponentPaletteRestore;
        AnimData_SelectPaletteBank(child->animation, owner->palette);
        *health -= target->damage;
        PlayerStats_AddContactHundredths(source, target, owner->contactHundredths);
        if (*health <= 0) {
            Sword_SetChildState(child, 1, 5);
            child->update = TargetMotion_UpdateFlashingExit;
            Sound_PlaySong(0x13E);
            return;
        }
    }

    phaseSnapshot = Sword_GetPhase(child, &phase);
    if ((phaseSnapshot == 2 && child->returnUpdate != Sword_DecelerateReturn) ||
        (phaseSnapshot == 3 && (child->thrustUpdate == Sword_UpdateThrustDelay ||
                                child->thrustUpdate == Sword_UpdateThrustExtension)) ||
        (phaseSnapshot == 4 && (child->lungeUpdate == Sword_UpdateLungeSpinup ||
                                child->lungeUpdate == Sword_UpdateSpinningLunge ||
                                child->lungeUpdate == TargetMotion_AdvanceFollowSegment))) {
        Sword_SetTrajectoryFromCurrentPosition(child);
        Sword_BeginContactReturn(child, phase);

        if (source->flags & 0x100) {
            child->direction = source->direction;
        } else {
            Sword_ContactDisplacement(&displacement, (void *)target->pos, (void *)source->pos);
            if (displacement.x == 0 && displacement.z == 0) {
                u32 *counter = &gRandomIndex;
                *counter = (*counter + 1) & 0x3FF;
                child->direction = ((const u16 *)RANDOM_TABLE_ADDR)[*counter];
            } else {
                child->direction = Math_Atan2(displacement.x, displacement.z);
            }
        }
        child->fieldA6 = 1;
        child->field52 = 1;
        child->force = source->force;
        child->velocityX = gUnk_086149C4[(child->direction + 0x40) & 0xFF] >> 7;
        child->velocityZ = gUnk_086149C4[child->direction] >> 7;
        Sound_PlaySong(0x170);
    } else {
        if (target->damage != 0)
            Sound_PlaySong(0x13E);
        else
            Sound_PlaySong(0x190);
    }
}
