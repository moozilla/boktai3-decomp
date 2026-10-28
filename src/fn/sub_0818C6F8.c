#include "global.h"
#include "ram.h"

#ifdef NONMATCHING
// fuzzy: 83.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: recoil address lifetimes, load scheduling, and scratch register allocation.

void Collision_CalculateDamageResponse(void *, void *);
void AnimData_SelectPaletteBank(void *, u32);
void Armor_UpdatePartPaletteRestore(void);
void Armor_DampPartWaveReturn(void);
void Armor_UpdatePartLungeDelay(void);
void Armor_UpdatePartLunge(void);
void SpinningApproach_UpdateSpinRamp(void);
void SpinningApproach_UpdateApproach(void);
void SpinningApproach_UpdateOwnerRelativeSegment(void);
void Armor_DampPartMovement(void);
void SpinningApproach_DampReturnMovement(void);
void Armor_CapturePartMotionEndpoint(void *);
void Sound_PlaySongChecked(u32);
s32 Math_Atan2(s32, s32);
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];

struct PartVector {
    s16 x, y, z, pad;
};
static inline void Delta(struct PartVector *out, struct PartVector *b, struct PartVector *a)
{
    typeof(a->y - b->y) t1;
    out->x = a->x - b->x;
    t1 = a->y - b->y;
    out->y = t1;
    out->z = a->z - b->z;
}

static inline u32 AttackFlags(u8 *attack, u32 mask)
{
    return *(u32 *)(attack + 0x34) & mask;
}
static inline void EnterBroken(u8 *part, u32 one, u32 mode)
{
    part[0x11E] = one;
    part[0x11F] = mode;
    part[0x119] = one | part[0x119];
}
inline static void EnterBroken_OnPartDamageCollision(u8 *part, u8 one, u32 mode)
{
    part[0x11E] = one;
    part[0x11F] = mode;
    {
        typeof(part[0x119]) v1 = part[0x119];
        part[0x119] = one | v1;
    }
}

void Armor_OnPartDamageCollision(u8 *attack, u8 *response, u8 *part)
{
    u8 *owner = *((u8 **)(part + 0x15C));
    s16 *hp = (s16 *)(part + 0x10A);
    void (**palette)(void);
    u8 *mode;
    void (*motion)(void);
    u32 angle;
    struct PartVector delta;
    if ((((*hp) <= 0) || AttackFlags(attack, 0x2000)) || (*((u16 *)(part + 0xE0))))
        return;
    Collision_CalculateDamageResponse(attack, response);
    {
        u32 value = *((u16 *)(attack + 0x44));
        *((u16 *)(response + 0x44)) = value;
    }
    palette = (void (**)(void))(part + 0x1CC);
    if ((!(*palette)) && (*((u16 *)(response + 0x3E)))) {
        *((u16 *)(part + 0x128)) = 8;
        *palette = Armor_UpdatePartPaletteRestore;
        AnimData_SelectPaletteBank(*((void **)(part + 0xC)), *((u16 *)(owner + 0x5BE)));
        *hp -= *((u16 *)(response + 0x3E));
        if ((*hp) <= 0) {
            EnterBroken_OnPartDamageCollision(part, 1, 5);
            goto hit;
        }
    }
    mode = part + 0x11F;
    if (((*mode) == 2) && ((*((void (**)(void))(part + 0x1D0))) != Armor_DampPartWaveReturn))
        goto recoil;
    motion = *((void (**)(void))(part + 0x1D0));
    if (((((motion == Armor_UpdatePartLungeDelay) || (motion == Armor_UpdatePartLunge)) ||
          (motion == SpinningApproach_UpdateSpinRamp)) ||
         (motion == SpinningApproach_UpdateApproach)) ||
        (motion == SpinningApproach_UpdateOwnerRelativeSegment)) {
    recoil:
        Armor_CapturePartMotionEndpoint(part);

        switch (*mode) {
            case 2:
                *((u16 *)(part + 0x1D6)) = 128;
                *((void (**)(void))(part + 0x1D0)) = Armor_DampPartWaveReturn;
                break;

            case 3:
                *((u16 *)(part + 0x1D6)) = 20;
                *((void (**)(void))(part + 0x1D0)) = Armor_DampPartMovement;
                break;

            case 4:
                *((u16 *)(part + 0x1D6)) = 100;
                *((void (**)(void))(part + 0x1D0)) = SpinningApproach_DampReturnMovement;
                break;
        }

        if ((*((u16 *)(attack + 6))) & 0x100) {
            angle = attack[0x42];
        } else {
            Delta(&delta, (void *)(response + 0xC), (void *)(attack + 0xC));
            if ((delta.x == 0) && (delta.z == 0)) {
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                angle = gUnk_0203B400[gRandomIndex];
            } else {
                angle = Math_Atan2(delta.x, delta.z);
            }
        }
        part[0x11A] = angle;
        *((u16 *)(part + 0xA6)) = 1;
        *((u16 *)(part + 0x52)) = 1;
        {
            u32 value = *((u16 *)(attack + 0x22));
            *((u16 *)(part + 0x6A)) = value;
        }
        {
            s32 value = gUnk_086149C4[(part[0x11A] + 64) & 255] >> 7;
            *((u16 *)(part + 0x100)) = value;
        }
        {
            s32 value = gUnk_086149C4[part[0x11A]] >> 7;
            *((u16 *)(part + 0x104)) = value;
        }
        Sound_PlaySongChecked(0x170);
    } else if (*((u16 *)(response + 0x3E))) {
    hit:
        Sound_PlaySongChecked(0x13E);

    } else {
        Sound_PlaySongChecked(0x190);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Armor_OnPartDamageCollision);
#endif
