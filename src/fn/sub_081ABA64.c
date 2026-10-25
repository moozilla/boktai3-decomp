#include "global.h"

/* Frame sequencer shared by the animation cases: a looping table of frame words. */
struct h10cFrame {
    u16 *table;
    u8 flags4;
    u8 count5;
    u8 value6;
    u8 limit7;
    u16 index8;
    u16 unkA;
    u16 scaleC;
    u16 timerE;
};

static inline void h10c_StepFrame(struct h10cFrame *f)
{
    u16 timer = f->timerE + 1;
    f->timerE = timer;
    if (timer >= f->limit7) {
        f->timerE = 0;
        if (f->flags4 & 4) {
            if (f->index8 != 0) {
                f->index8--;
            } else if (f->flags4 & 8) {
                f->index8 = f->count5 - 1;
            } else {
                f->index8 = f->count5;
            }
        } else {
            f->index8++;
            if (f->index8 >= f->count5) {
                if (f->flags4 & 8) {
                    f->timerE = 0;
                    f->index8 = f->count5 - 1;
                }
            }
        }
    }
    f->value6 = f->table[f->index8] & 0xF;
    f->limit7 = (u8)((f->value6 * f->scaleC) >> 6);
    if (f->limit7 == 0) {
        f->limit7 = 1;
    }
}

// confidence=low: Vanargand boss action state machine, dispatched on the phase byte at +0xA2

s32 Vanargand_GetTargetLateralDistance(void *);

/* Frame sequencer for the animation held at +0x298: advances a looping table of frame words and
   mirrors the current frame into the drawable's flags. Returns 1 when the sequence ended. */

/* Writes the three s16 offsets derived from two packed s16 values (a, b) */

// confidence=low: Vanargand boss action state machine, dispatched on the phase byte at +0xA2

/* Frame sequencer for the animation held at +0x298: advances a looping table of frame words and
   mirrors the current frame into the drawable's flags. Returns 1 when the sequence ended. */

/* Writes the three s16 offsets derived from two packed s16 values (a, b) */

// confidence=low: Vanargand boss action state machine, dispatched on the phase byte at +0xA2
void Vanargand_SetPendingCue(void *, u16);
s32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Debug_NudgeWorldPositionWithA(void *);
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void VanargandAfterimagePool_Spawn(void *, void *);
void Vanargand_StartActionSequence(void *);
void Vanargand_CreateTransientEffect(void *, u8);
void Vanargand_AdvanceEffectStageOne(void *);
void Vanargand_AdvanceEffectStageThree(void *);
void Sprite_ApplyAnimation(void *, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void PlayerContact_ApplySyntheticContact(void *, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
s32 sub_081A6AEC(void *);
s32 sub_081A6A08(void *);
extern const s16 gUnk_086149C4[];

#define VG_AT(T, off) (*(T *)(base + (off)))

/* Frame sequencer for the animation held at +0x298: advances a looping table of frame words and
   mirrors the current frame into the drawable's flags. Returns 1 when the sequence ended. */
struct Vg_FrameSeq {
    u16 *table;
    u8 flags4;
    u8 count5;
    u8 value6;
    u8 limit7;
    u16 index8;
    u16 unkA;
    u16 scaleC;
    u16 timerE;
};

static inline u32 Vg_081ABA64_StepFrame(struct Vg_FrameSeq *f, u32 *target)
{
    u16 *frame = f->table + f->index8;
    u32 done = 0;
    u16 tmr;

    ((u16 *)target)[8] = *frame >> 6;
    if ((f->flags4 & 1) != ((*frame >> 4) & 1))
        *target |= 4;
    else
        *target &= ~4;
    if ((f->flags4 & 2) != ((*frame >> 4) & 2))
        *target |= 8;
    else
        *target &= ~8;
    tmr = f->timerE + 1;
    f->timerE = tmr;
    if (tmr >= f->limit7) {
        f->timerE = 0;
        if (f->flags4 & 4) {
            if (f->index8 == 0) {
                if (f->flags4 & 8)
                    f->timerE = f->limit7;
                else
                    f->index8 = f->count5 - 1;
                done = 1;
            } else {
                f->index8 = f->index8 - 1;
            }
        } else {
            f->index8 = f->index8 + 1;
            if ((u16)f->index8 >= f->count5) {
                if (f->flags4 & 8) {
                    f->timerE = f->limit7;
                    f->index8 = f->count5 - 1;
                } else {
                    f->index8 = 0;
                }
                done = 1;
            }
        }
        frame = f->table + f->index8;
        f->value6 = *frame & 0xF;
        f->limit7 = (f->value6 * f->scaleC) >> 6;
        if (f->limit7 == 0)
            f->limit7 = 1;
    }
    return done;
}

/* Writes the three s16 offsets derived from two packed s16 values (a, b) */
static inline void Vg_081ABA64_SetOffsets(s16 *dst, u32 value)
{
    volatile u32 packed = value;
    s16 a = ((volatile s16 *)&packed)[0];
    s16 b = ((volatile s16 *)&packed)[1];
    s32 t;

    t = ((a + b) * 32) / 3 + 0x40;
    dst[0] = t / 2;
    dst[1] = 0x40;
    t = ((b - a) * 32) / 3 + 0x40;
    dst[2] = t / 2;
}

static inline void Vg_081ABA64_Contact(u8 *base)
{
    if (sub_081A6A08(base)) {
        if (VG_AT(u8, 0x230) == 0)
            PlayerContact_ApplySyntheticContact(VG_AT(void *, 0x10C), VG_AT(u16, 0x58E), 2, 0x20, 0,
                                                1);
        else
            PlayerContact_ApplySyntheticContact(VG_AT(void *, 0x10C), VG_AT(u16, 0x58E), 2, 0x20, 0,
                                                2);
    }
}

#ifdef NONMATCHING
// fuzzy: 34.0%
// scope shape 946 (differences once low registers are set aside)
// confidence=low: Vanargand boss action state machine, dispatched on the phase byte at +0xA2
void sub_081ABA64(void *arg0)
{
    u8 *base = arg0;
    u8 *sub = base + 0x18C;
    u8 *counter = base + 0xA4;
    u8 *phaseP = base + 0xA2;
    struct Vg_FrameSeq *seq = (struct Vg_FrameSeq *)(base + 0x298);
    u32 *target = (u32 *)(sub + 0xA8);
    u8 r4;
    s16 org[4];
    u32 packed;
    s32 d, ang;
    s32 i;

    VG_AT(u32, 0x9C) |= 8;
    Vanargand_SetPendingCue(arg0, 1);
    r4 = Callback_InvokeActor(arg0, VG_AT(void *, 0x104));
    if (r4 != 0)
        return;

    if (VG_AT(u8, 0xB1) != 0) {
        VG_AT(u8, 0xB1) = r4;
        PairedSceneActor_ApplyToSelectedHalves(VG_AT(void *, 0x56C), 3, 0x13);
        *phaseP = 1;
        *(u32 *)counter = r4;
    }
    if (*phaseP > 10)
        return;

    switch (*phaseP) {
        case 0:
            Debug_NudgeWorldPositionWithA(base + 0x54);
            if ((*(u16 *)(0x03005260 + 2) & 0x100) == 0)
                return;
            if (*(u16 *)0x03005260 & 1)
                sub[0xA4] = 1;
            else
                sub[0xA4] = 0;
            *phaseP = 2;
            *(u32 *)counter = 0;
            return;
        case 1:
            if (*(u32 *)counter == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 2, 1, 0);
                d = sub_081A6AEC(arg0);
                if (d > 0x200)
                    d = 0x200;
                ang = VG_AT(u8, 0xCC);
                i = 0xE0;
                if ((u32)(ang - 0x20) <= 0x7F)
                    i = 0x60;
                org[0] = (gUnk_086149C4[(i + 0x40) & 0xFF] * d) / 0x1000;
                org[1] = 0;
                org[2] = (gUnk_086149C4[i] * d) / 0x1000;
                org[0] += 0x380;
                org[1] = 0x300;
                org[2] += 0x380;
                MotionTrack_Initialize(base + 0x534, base + 0x54, org, 0, 4);
                *(u32 *)counter += 1;
                Anim_SetSequence(base + 0x444, VG_AT(void *, 0x454), 0, 0, 0);
                VG_AT(u16, 0x500) = 1;
                VG_AT(u32, 0x458) = 0;
            }
            MotionTrack_Update(base + 0x534);
            if (VG_AT(u8, 0x55D) == 2) {
                *phaseP = 2;
                *(u32 *)counter = 0;
                return;
            }
            VanargandAfterimagePool_Spawn(VG_AT(void *, 0xF44), base + 0x1AC);
            return;
        case 2:
            if (*(u32 *)counter == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 0xA, 2, 0);
                if (sub[0xA4] == 0) {
                    VG_AT(u32, 0xF1C) = 0;
                    VG_AT(u16, 0xF20) = 2;
                    VG_AT(u16, 0xF22) = 6;
                } else {
                    VG_AT(u32, 0xF1C) = 0;
                    VG_AT(u16, 0xF20) = 3;
                    VG_AT(u16, 0xF22) = 6;
                }
                VG_AT(u32, 0xF18) = 0;
            }
            if (*(u32 *)counter == 7) {
                Sprite_ApplyAnimation(sub + 0xA8, sub + 0xD4);
                ((u16 *)(sub + 0xA8))[8] = 0x18;
                Vg_081ABA64_SetOffsets((s16 *)(sub + 0x128), 0);
            } else if (*(u32 *)counter == 0xA) {
                Vg_081ABA64_SetOffsets((s16 *)(sub + 0x128), 0x10000);
            }
            if (*(u32 *)counter > 6)
                VG_AT(u32, 0x9C) |= 0x80000;
            if (VG_AT(u8, 0xB2) != 0) {
                *phaseP = 3;
                *(u32 *)counter = 0;
                return;
            }
            *(u32 *)counter += 1;
            return;
        case 3:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0) {
                Anim_SetSequence(seq, *(void **)(sub + 0xA8 + 0x74), 3, 0, 0);
                *(u32 *)counter += 1;
            }
            if (*(u32 *)counter == 1) {
                Vg_081ABA64_StepFrame(seq, target);
                return;
            }
            *(u32 *)counter += 1;
            if (*(u32 *)counter <= 10)
                return;
            *phaseP = 4;
            *(u32 *)counter = 0;
            return;
        case 4:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0) {
                Anim_SetSequence(seq, *(void **)(sub + 0xA8 + 0x74), 4, 0, 0);
                *(u32 *)counter += 1;
                if (VG_AT(u32, 0xF24) == 1)
                    Sound_PlaySong(0x543);
            }
            if (!Vg_081ABA64_StepFrame(seq, target)) {
                *(u32 *)counter += 1;
                return;
            }
            if (*(u32 *)counter <= 0xA) {
                *(u32 *)counter += 1;
                return;
            }
            *phaseP = 5;
            *(u32 *)counter = 0;
            return;
        case 5:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 0xB, 2, 0);
                Anim_SetSequence(seq, *(void **)(sub + 0xA8 + 0x74), 5, 0, 0);
                Vg_081ABA64_SetOffsets((s16 *)(sub + 0x128), 0x30000);
                Vg_081ABA64_StepFrame(seq, target);
                Vanargand_StartActionSequence(base + 0x21C);
                Vanargand_CreateTransientEffect(base + 0x250, sub[0xA4]);
                Sound_PlaySong(0x584);
            }
            *(u32 *)counter += 1;
            if (*(u32 *)counter <= 0x3C)
                return;
            *phaseP = 6;
            *(u32 *)counter = 0;
            if (VG_AT(u8, 0x230) == 0)
                ScreenTransition_SendCommand(0, 3, 0x1F, 0x1F, 0x1F, 0xFFFF, 0);
            else
                ScreenTransition_SendCommand(0, 3, 4, 4, 4, 0xFFFF, 0);
            return;
        case 6:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 0xB, 2, 0);
                Vanargand_AdvanceEffectStageOne(base + 0x21C);
                org[0] = VG_AT(s16, 0x54) - 0x100;
                org[1] = VG_AT(s16, 0x56);
                org[2] = VG_AT(s16, 0x58) - 0x100;
                MotionTrack_Initialize(base + 0x534, base + 0x54, org, 0, 4);
            }
            Vg_081ABA64_StepFrame(seq, target);
            Vg_081ABA64_Contact(base);
            MotionTrack_Update(base + 0x534);
            if (VG_AT(u8, 0x231) == 3) {
                *phaseP = 7;
                *(u32 *)counter = 0;
                return;
            }
            *(u32 *)counter += 1;
            return;
        case 7:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0)
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 0xB, 2, 0);
            Vg_081ABA64_StepFrame(seq, target);
            if (*(u32 *)counter <= 0x3B) {
                Vg_081ABA64_Contact(base);
                if (*(u32 *)counter & 1) {
                    VG_AT(s16, 0x5C) = -2;
                    VG_AT(s16, 0x5E) = 0;
                    VG_AT(s16, 0x60) = -2;
                }
            }
            if (*(u32 *)counter == 0x3C)
                Vanargand_AdvanceEffectStageThree(base + 0x21C);
            if (VG_AT(u8, 0x231) == 0) {
                *phaseP = 8;
                *(u32 *)counter = 0;
                return;
            }
            *(u32 *)counter += 1;
            return;
        case 8:
            VG_AT(u32, 0x9C) |= 0x80000;
            if (*(u32 *)counter == 0) {
                Anim_SetSequence(seq, *(void **)(sub + 0xA8 + 0x74), 6, 0, 0);
                *(u32 *)counter += 1;
            }
            if (*(u32 *)counter == 1) {
                Vg_081ABA64_StepFrame(seq, target);
                return;
            }
            VG_AT(s16, 0x5C) = *(u32 *)counter;
            VG_AT(s16, 0x5E) = 0;
            VG_AT(s16, 0x60) = *(u32 *)counter;
            *(u32 *)counter += 1;
            if (*(u32 *)counter <= 0xF)
                return;
            *phaseP = 9;
            *(u32 *)counter = 0;
            return;
        case 9:
            if (*(u32 *)counter == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub + 0x60, 0xA, 2, 2);
                Sprite_ApplyAnimation(sub + 0xA8, sub + 0xD4);
                ((u16 *)(sub + 0xA8))[8] = 0x18;
                Vg_081ABA64_SetOffsets((s16 *)(sub + 0x128), 0x10000);
                if (sub[0xA4] == 0) {
                    VG_AT(u32, 0xF1C) = 0;
                    VG_AT(u16, 0xF20) = 4;
                    VG_AT(u16, 0xF22) = 5;
                } else {
                    VG_AT(u32, 0xF1C) = 0;
                    VG_AT(u16, 0xF20) = 5;
                    VG_AT(u16, 0xF22) = 5;
                }
                VG_AT(u32, 0xF18) = 0;
                Anim_SetSequence(base + 0x444, VG_AT(void *, 0x454), 0, 0, 4);
                VG_AT(u16, 0x500) = 1;
                VG_AT(u32, 0x458) = 0;
            }
            if (*(u32 *)counter == 3)
                Vg_081ABA64_SetOffsets((s16 *)(sub + 0x128), 0);
            if (*(u32 *)counter > 6)
                VG_AT(u32, 0x9C) |= 0x80000;
            if (VG_AT(u8, 0xB2) != 0) {
                *phaseP = 10;
                *(u32 *)counter = 0;
                return;
            }
            *(u32 *)counter += 1;
            return;
        case 10:
            VG_AT(u8, 0xAF) = 1;
            *phaseP = 0;
            *(u32 *)counter = 0;
            return;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081ABA64);
#endif
