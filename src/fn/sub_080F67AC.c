#include "global.h"

// confidence=low: sword task setup; reads script parameters, initializes the main sprite, shadow, colliders and four sword segments
#define FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define READ_OPT()          (Script_GetPc() ? Script_GetValue() : 0)

// confidence=low: sword task setup; reads script parameters, initializes the main sprite, shadow, colliders and four sword segments
#define FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define READ_OPT()          (Script_GetPc() ? Script_GetValue() : 0)

// confidence=low: sword task setup; reads script parameters, initializes the main sprite, shadow, colliders and four sword segments
#define FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define READ_OPT()          (Script_GetPc() ? Script_GetValue() : 0)

// confidence=low: sword task setup; reads script parameters, initializes the main sprite, shadow, colliders and four sword segments
#define FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define READ_OPT()          (Script_GetPc() ? Script_GetValue() : 0)

extern void *gUnknown_02000580;

void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u32, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void ScaledShadow_Initialize(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(u32, s32);
s32 Terrain_GetHeightAt(void *);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_Refresh(void *);
void Ivy_InitPaletteBuffers(void *);
void Ivy_SetSegmentProperties(void *, void *, u32);
void ChildGroup_ReadScriptParameters(void *);
void TargetRegistry_RegisterSceneActor(void *, void *, void *);
void AnimData_SelectPaletteBank(void *, u32);
extern void Ivy_OnDestroy(void);
extern void ColorPulse_ApplyContact(void);
extern void ChildGroup_ProcessEvent(void);
extern void sub_080F20AC(void);
extern void sub_080F3B30(void);
extern void Ivy_InitColorTransition(void);

static inline void AdvanceAnim080F67AC(u8 *anim)
{
    u32 timer;
    u32 duration;
    u32 frameIndex;
    u16 *frame;

    timer = (u16)(FIELD(anim, 0xE, u16) + 1);
    FIELD(anim, 0xE, u16) = timer;
    duration = FIELD(anim, 7, u8);
    if (timer >= duration) {
        FIELD(anim, 0xE, u16) = 0;
        if (FIELD(anim, 4, u8) & 4) {
            if (FIELD(anim, 8, u16) == 0) {
                if (FIELD(anim, 4, u8) & 8) {
                    FIELD(anim, 0xE, u16) = duration;
                    goto frame_done;
                }
                frameIndex = FIELD(anim, 5, u8) - 1;
            } else {
                frameIndex = FIELD(anim, 8, u16) - 1;
            }
            FIELD(anim, 8, u16) = frameIndex;
            goto frame_done;
        }
        frameIndex = FIELD(anim, 8, u16) + 1;
        if ((u16)frameIndex < FIELD(anim, 5, u8)) {
            FIELD(anim, 8, u16) = frameIndex;
        } else {
            if (FIELD(anim, 4, u8) & 8) {
                FIELD(anim, 0xE, u16) = duration;
                frameIndex = FIELD(anim, 5, u8) - 1;
            } else {
                frameIndex = 0;
            }
            FIELD(anim, 8, u16) = frameIndex;
        }
    }
frame_done:
    frame = (u16 *)((u8 *)FIELD(anim, 0, void *) + (FIELD(anim, 8, u16) << 1));
    FIELD(anim, 6, u8) = *frame & 0xF;
    FIELD(anim, 7, u8) = (FIELD(anim, 0xC, u16) * FIELD(anim, 6, u8)) >> 6;
    if (FIELD(anim, 7, u8) == 0)
        FIELD(anim, 7, u8) = 1;
}

static inline void SyncSpriteFlag(u32 *flags, u8 anim4, u16 frame, u32 bit, u32 mask)
{
    if ((anim4 & mask) != (((frame & 0x30) >> 4) & mask))
        *flags |= bit;
    else
        *flags &= ~bit;
}

#ifdef NONMATCHING
// fuzzy: 52.6%
// scope shape 468 (differences once low registers are set aside)
void sub_080F67AC(struct SwordTask *p)
{
    u8 *anim;
    u8 *spriteMain;
    u8 *animMain;
    u32 hit[2];
    u32 box[2];
    s32 i;
    s32 sb;
    u16 *frame;

    FIELD(p, 0x60, void *) = gUnknown_02000580;
    FIELD(p, 0x64, u16) = 0;
    FIELD(p, 0x66, u16) = 0;
    FIELD(p, 0x68, u16) = 0;
    FIELD(p, 0x6C, u16) = 0;
    FIELD(p, 0x6E, u16) = 1;
    FIELD(p, 0x70, u16) = 0;
    FIELD(p, 0x8D, u8) = 0;
    FIELD(p, 0x7E, u16) = 0;
    FIELD(p, 0x8E, u8) = 0;
    FIELD(p, 0x9E, u8) = 0;
    FIELD(p, 0x9F, u8) = 0;
    FIELD(p, 0x9A, u16) = 0;
    FIELD(p, 0x9C, u16) = 0;
    FIELD(p, 0x92, u8) = 1;
    FIELD(p, 0x93, u8) = 0;
    FIELD(p, 0x8D, u8) |= 1;
    FIELD(p, 0xCEC, u32) = 0;
    FIELD(p, 0xCF8, u32) = 0x14;
    FIELD(p, 0xCC8, void *) = sub_080F3B30;
    FIELD(p, 0xCF0, void *) = Ivy_InitColorTransition;

    animMain = (u8 *)p + 0xB94;
    spriteMain = (u8 *)p + 0xB68;
    AnimData_Load(animMain, 0x5291);
    Sprite_Init(spriteMain, animMain, 0);
    {
        void *script = ScriptProc_Start(0x922E, 0x3F72);
        anim = (u8 *)p + 0xB58;
        Anim_SetSequence(anim, script, 0, FIELD(p, 0xD55, u8), 0);
    }

    frame = (u16 *)((u8 *)FIELD(anim, 0, void *) + (FIELD(anim, 8, u16) << 1));
    FIELD(spriteMain, 0x10, u16) = *frame >> 6;
    SyncSpriteFlag((u32 *)spriteMain, FIELD(anim, 4, u8), *frame, 4, 1);
    SyncSpriteFlag((u32 *)spriteMain, FIELD(anim, 4, u8), *frame, 8, 2);
    AdvanceAnim080F67AC(anim);
    FIELD(spriteMain, 0, u32) |= 2;

    AnimData_SelectPaletteBank(FIELD(p, 0xB74, void *), FIELD(p, 0xD56, u16));

    {
        u16 *src = FIELD(animMain, 0xC, u16 *);
        u16 *dst = (u16 *)((u8 *)p + 0xD30);
        for (i = 15; i >= 0; i--) {
            *dst++ = *src++;
        }
        FIELD(animMain, 0xC, u16 *) = (u16 *)((u8 *)p + 0xD30);
    }
    FIELD(p, 0xD52, u8) = 5;
    FIELD(p, 0xD53, u8) = 0;
    FIELD(p, 0xD54, u8) = 0;

    ScaledShadow_Initialize((u8 *)p + 0xBB0, (u8 *)p + 0xB84, 0x30, 0x30, 0x30, 0, 0, 0, 0, 0, 0);

    if (Script_SeekToKeyword(0x41)) {
        FIELD(p, 0x7E, s16) = READ_OPT();
        FIELD(p, 0x80, u16) = READ_OPT();
        FIELD(p, 0x82, u16) = READ_OPT();
        FIELD(p, 0x84, u16) = READ_OPT();
        FIELD(p, 0x8A, u16) = READ_OPT();
        FIELD(p, 0x88, u16) = READ_OPT();
        FIELD(p, 0xD5C, u16) = FIELD(p, 0x7E, u16);
    }
    if (Script_SeekToKeyword(0x54)) {
        for (sb = 1; sb >= 0; sb--) {
            i = sb * 2;
            FIELD(p, 0xD04 + i, s16) = READ_OPT();
            FIELD(p, 0xD08 + i, s16) = READ_OPT();
            FIELD(p, 0xD0C + i, s16) = READ_OPT();
            FIELD(p, 0xD10 + i, s16) = READ_OPT();
        }
    }
    if (Script_SeekToKeyword(0x70)) {
        FIELD(spriteMain, 0x1C, s16) = Script_GetValue();
        FIELD(spriteMain, 0x1E, s16) = Script_GetValue();
        FIELD(spriteMain, 0x20, s16) = Script_GetValue();
    } else {
        FIELD(spriteMain, 0x1C, s16) = 0;
        FIELD(spriteMain, 0x1E, s16) = 0;
        FIELD(spriteMain, 0x20, s16) = 0;
    }
    FIELD(p, 0xD5E, u8) = Script_GetKeywordValue(0x4B, 2);
    FIELD(p, 0xD86, u16) = Script_GetKeywordValue(0x56, -1);
    FIELD(spriteMain, 0x1E, s16) = Terrain_GetHeightAt(spriteMain + 0x1C) + 0x15E;

    hit[0] = (((hit[0] & 0xFFFF0000) | 0x32) & 0xFFFF) | (0xC8 << 14);
    hit[1] = (hit[1] & 0xFFFF0000) | 0x32;
    box[0] = 0;
    box[1] &= 0xFFFF0000;

    Collider_Init((u8 *)p + 0xC1C, 0, 0x2001, 0, 0x10, hit, box);
    Collider_SetAttackProperties((u8 *)p + 0xC1C, FIELD(p, 0xD04, s16), FIELD(p, 0xD08, s16), 0,
                                 FIELD(p, 0xD0C, s16), FIELD(p, 0xD10, s16));
    Collider_SetCallback((u8 *)p + 0xC1C, Ivy_OnDestroy, p);
    Collider_Refresh((u8 *)p + 0xC1C);
    FIELD(p, 0xC22, u16) &= 0xFFFB;

    ((s16 *)hit)[0] = 0x40;
    ((s16 *)hit)[1] = 0x40;
    ((s16 *)hit)[2] = 0x40;
    Collider_Init((u8 *)p + 0xC70, 0, 0x5009, 0, 0x10, hit, box);
    Collider_SetBodyProperties((u8 *)p + 0xC70, FIELD(p, 0x80, s16), FIELD(p, 0x8A, s16),
                               FIELD(p, 0x88, s16));
    Collider_SetCallback((u8 *)p + 0xC70, ColorPulse_ApplyContact, p);
    Collider_Refresh((u8 *)p + 0xC70);
    FIELD(p, 0xCD0, void *) = sub_080F20AC;

    for (sb = 0; sb <= 3; sb++) {
        u8 *sprite;
        u8 *col;
        u8 *anim2 = (u8 *)p + 0xB58;
        u8 *animSeg = (u8 *)p + 0x158 + sb * 0x1C;
        AnimData_Load(animSeg, 0x5291);
        sprite = (u8 *)p + 0xA8 + sb * 0x2C;
        Sprite_Init(sprite, animSeg, 0);
        frame = (u16 *)((u8 *)FIELD(anim2, 0, void *) + (FIELD(anim2, 8, u16) << 1));
        FIELD(sprite, 0x10, u16) = *frame >> 6;
        SyncSpriteFlag((u32 *)sprite, FIELD(anim2, 4, u8), *frame, 4, 1);
        SyncSpriteFlag((u32 *)sprite, FIELD(anim2, 4, u8), *frame, 8, 2);
        AdvanceAnim080F67AC(anim2);

        FIELD(p, 0xA8 + sb * 0x2C, u32) = 0x203;
        FIELD(p, 0xAF + sb * 0x2C, u8) = 3;
        FIELD(p, 0xC4 + sb * 0x2C, u16) = 0;
        FIELD(p, 0xC6 + sb * 0x2C, u16) = 0;
        FIELD(p, 0xC8 + sb * 0x2C, u16) = 0;

        col = (u8 *)p + sb * 0x54 + 0x1C8;
        Collider_Init(col, 0, 0x2001, 0, 0x10, hit, box);
        Collider_SetCallback(col, 0, p);
        Collider_Refresh(col);
        Collider_SetAttackProperties(col, FIELD(p, 0xD06, s16), FIELD(p, 0xD0A, s16), 0,
                                     FIELD(p, 0xD0E, s16), FIELD(p, 0xD12, s16));

        col = (u8 *)p + sb * 0x54;
        FIELD(col, 0x1CE, u16) |= 4;
        col = (u8 *)p + sb * 0x54 + 0x318;
        Collider_Init(col, 0, 0x4009, 0, 0x10, hit, box);
        Collider_SetBodyProperties(col, FIELD(p, 0x80, s16), FIELD(p, 0x8A, s16),
                                   FIELD(p, 0x88, s16));
        Collider_SetCallback(col, 0, p);
        Collider_Refresh(col);
        FIELD(p, sb * 0x54 + 0x31E, u16) |= 4;
    }

    Ivy_InitPaletteBuffers(p);
    Ivy_SetSegmentProperties(p, (u8 *)p + 0xB0C, 0x13C);
    ChildGroup_ReadScriptParameters(p);
    Script_GetKeywordValue(0x4E, 0);
    if (Script_GetKeywordValue(0x4D, 0) & 0x200)
        TargetRegistry_RegisterSceneActor((u8 *)p + 0xD88, (u8 *)p + 0xC70,
                                          ChildGroup_ProcessEvent);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F67AC);
#endif
