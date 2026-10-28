#include "global.h"
#include "render.h"
#include "input.h"

struct Keys {
    u16 held;
    u16 pressed;
};
struct FrameTable {
    s32 v[4];
};
extern const struct FrameTable gUnk_0824FF98;

void Sound_PlaySong(u32);
u32 SceneGrid_IsCellBeforeLimit(s32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 ForgingBook_GetCategoryEntryOffset(s32);
void ForgingBook_RefreshCategory(void *);
void ForgingBook_RefreshSelection(void *);
void ForgingBook_UpdateHighlightPulse(void *);
s32 ForgingBook_SetAnimation(void *, s32, s32, s32);
void PlayerAttackEffect_SetEmissionState(s32);
s32 Anim_LoadKeyframe(void *, void *, u16, u32);
void Anim_SetKeyframes(void *, void *, u16, u32, u32);
void Script_Call(void *, u32);
void Task_Kill(void *);

struct ForgingBookPreview {
    u8 unknown[0xAA8];
    s16 previewX;
    u8 unknownAAA[2];
    s16 previewY;
};
#define FRAME       (*(u32 *)(m + 0x20))
#define ANIM_DATA   (m + 0xa68)
#define ANIM        (m + 0xa88)
#define PREVIEW_X   (((struct ForgingBookPreview *)m)->previewX)
#define PREVIEW_Y   (((struct ForgingBookPreview *)m)->previewY)
#define ANIM_BASE   (*(u8 *)(m + 0xae8))
#define ANIM_AE9    (*(u8 *)(m + 0xae9))
#define ANIM_FRAME  (*(s16 *)(m + 0xaea))
#define STEP        (*(s16 *)(m + 0xb0c))
#define CATEGORY    (*(s16 *)(m + 0xb60))
#define COL         (*(s16 *)(m + 0xb64))
#define ROW         (*(s16 *)(m + 0xb66))
#define INDEX       (*(u16 *)(m + 0xb68))
#define EXITING     (*(s16 *)(m + 0xb6c))
#define FADE        (*(s16 *)(m + 0xb6e))
#define TIMER       (*(s16 *)(m + 0xb70))
#define ANIM_BE5    (*(u8 *)(m + 0xbe5))
#define EXIT_SCRIPT (*(void **)(m + 0xbe8))

s32 sub_0821290C(u8 *m)
{
    u32 catChanged = 0;
    u32 moved = 0;

    if (EXITING == 0) {
        if ((*(struct Keys *)gPadState).pressed & 2) {
            ScreenTransition_SendCommand(1, FADE, 4, 4, 4, 0xffff, moved);
            EXITING = 1;
            Sound_PlaySong(0x119);
        } else {
            if ((*(struct Keys *)gPadState).pressed & 0x200) {
                if (--CATEGORY < 0)
                    CATEGORY = 9;
                COL = moved;
                ROW = moved;
                Sound_PlaySong(0x10a);
                catChanged = 1;
            } else if ((*(struct Keys *)gPadState).pressed & 0x100) {
                if (++CATEGORY > 9)
                    CATEGORY = moved;
                COL = moved;
                ROW = moved;
                Sound_PlaySong(0x10a);
                catChanged = 1;
            }
            if (((*(struct Keys *)gPadState).pressed & 0x40) &&
                (SceneGrid_IsCellBeforeLimit(COL, ROW - 1, CATEGORY))) {
                ROW--;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if (((*(struct Keys *)gPadState).pressed & 0x80) &&
                       (SceneGrid_IsCellBeforeLimit(COL, ROW + 1, CATEGORY))) {
                ROW++;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if (((*(struct Keys *)gPadState).pressed & 0x20) &&
                       (SceneGrid_IsCellBeforeLimit(COL - 1, ROW, CATEGORY))) {
                COL--;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if (((*(struct Keys *)gPadState).pressed & 0x10) &&
                       (SceneGrid_IsCellBeforeLimit(COL + 1, ROW, CATEGORY))) {
                COL++;
                moved = 1;
                Sound_PlaySong(0xdc);
            }
        }
    }
    if (catChanged || moved) {
        u16 idx = ForgingBook_GetCategoryEntryOffset(CATEGORY) + ROW * 6 + COL;
        INDEX = idx;
    }
    if (catChanged)
        ForgingBook_RefreshCategory(m);
    if (moved)
        ForgingBook_RefreshSelection(m);
    if (EXITING) {
        if (TIMER > 1 << FADE) {
            if (EXIT_SCRIPT)
                Script_Call(EXIT_SCRIPT, 0);
            Task_Kill(m);
        }
        TIMER++;
    } else {
        struct FrameTable tbl = gUnk_0824FF98;
        s32 done;
        s16 *frame;
        s16 *category;

        ForgingBook_UpdateHighlightPulse(m);
        frame = &ANIM_FRAME;
        done = ForgingBook_SetAnimation(m, *frame, 0x40, ANIM_BE5);
        category = &CATEGORY;
        if (*category <= 5) {
            if (FRAME == 0x28) {
                *frame = 0;
                Anim_LoadKeyframe(ANIM, ANIM_DATA, ANIM_BASE, 2);
                Anim_SetKeyframes(ANIM, ANIM_DATA, *frame + ANIM_BASE, 2, 4);
            }
            if (*category == 0 && done && STEP <= 1 && FRAME < 0x28) {
                s32 v;
                STEP++;
                v = tbl.v[STEP];
                *frame = v;
                Anim_LoadKeyframe(ANIM, ANIM_DATA, v += ANIM_BASE, 2);
                Anim_SetKeyframes(ANIM, ANIM_DATA, *frame + ANIM_BASE, 2, 4);
            }
            if (ANIM_FRAME == 0x54)
                PlayerAttackEffect_SetEmissionState(1);
            else if (ANIM_FRAME == 0x72)
                PlayerAttackEffect_SetEmissionState(3);
            else if (ANIM_FRAME != 0x5e)
                PlayerAttackEffect_SetEmissionState(2);
        } else {
            if (FRAME == 0x28) {
                PREVIEW_X = ((u16)gCamera.position.x) - 456;
                PREVIEW_Y = ((u16)gCamera.position.z) + 336;
                *frame = 0;
                ANIM_BASE = 3;
                ANIM_AE9 = 1;
                Anim_LoadKeyframe(ANIM, ANIM_DATA, *frame + ANIM_BASE, 2);
                Anim_SetKeyframes(ANIM, ANIM_DATA, *frame + ANIM_BASE, 2, 4);
            }
        }
    }
    FRAME++;
}
