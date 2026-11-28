#include "global.h"
#include "engine/input.h"
#include "engine/video/render.h"
#include "gba/io_reg.h"
#include "ui/books/forging_book.h"

struct FrameTable {
    s32 v[4];
};
extern const struct FrameTable gUnk_0824FF98;

void Sound_PlaySong(u32);
u32 SceneGrid_IsCellBeforeLimit(s32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void ForgingBook_RefreshCategory(struct ForgingBook *);
void ForgingBook_RefreshSelection(struct ForgingBook *);
void ForgingBook_UpdateHighlightPulse(struct ForgingBook *);
s32 ForgingBook_SetAnimation(struct ForgingBook *, s32, s32, s32);
void PlayerAttackEffect_SetEmissionState(s32);

s32 sub_0821290C(struct ForgingBook *book)
{
    u32 categoryChanged = 0;
    u32 moved = 0;

    if (book->exiting == 0) {
        if (gPadState[1] & B_BUTTON) {
            ScreenTransition_SendCommand(1, book->fadeShift, 4, 4, 4, 0xffff, 0);
            book->exiting = 1;
            Sound_PlaySong(0x119);
        } else {
            if (gPadState[1] & L_BUTTON) {
                if (--book->category < 0)
                    book->category = 9;
                book->column = 0;
                book->row = 0;
                Sound_PlaySong(0x10a);
                categoryChanged = 1;
            } else if (gPadState[1] & R_BUTTON) {
                if (++book->category > 9)
                    book->category = 0;
                book->column = 0;
                book->row = 0;
                Sound_PlaySong(0x10a);
                categoryChanged = 1;
            }
            if ((gPadState[1] & DPAD_UP) &&
                SceneGrid_IsCellBeforeLimit(book->column, book->row - 1, book->category)) {
                book->row--;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if ((gPadState[1] & DPAD_DOWN) &&
                       SceneGrid_IsCellBeforeLimit(book->column, book->row + 1, book->category)) {
                book->row++;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if ((gPadState[1] & DPAD_LEFT) &&
                       SceneGrid_IsCellBeforeLimit(book->column - 1, book->row, book->category)) {
                book->column--;
                moved = 1;
                Sound_PlaySong(0xdc);
            } else if ((gPadState[1] & DPAD_RIGHT) &&
                       SceneGrid_IsCellBeforeLimit(book->column + 1, book->row, book->category)) {
                book->column++;
                moved = 1;
                Sound_PlaySong(0xdc);
            }
        }
    }
    if (categoryChanged || moved)
        book->selectedEntry = ForgingBook_GetCategoryEntryOffset(book->category) + book->row * 6 +
                              book->column;
    if (categoryChanged)
        ForgingBook_RefreshCategory(book);
    if (moved)
        ForgingBook_RefreshSelection(book);
    if (book->exiting) {
        if (book->fadeTimer > 1 << book->fadeShift) {
            if (book->exitScript)
                Script_Call(book->exitScript, 0);
            Task_Kill(book);
        }
        book->fadeTimer++;
    } else {
        struct FrameTable frames = gUnk_0824FF98;
        s32 done;

        ForgingBook_UpdateHighlightPulse(book);
        done = ForgingBook_SetAnimation(book, book->animationIndex, 0x40, book->animationMode);
        if (book->category <= 5) {
            if (book->frame == 0x28) {
                book->animationIndex = 0;
                Anim_LoadKeyframe(&book->preview, &book->previewResource, book->animationBase, 2);
                Anim_SetKeyframes(&book->preview, &book->previewResource,
                                  (u16)(book->animationIndex + book->animationBase), 2, 4);
            }
            if (book->category == 0 && done && book->previewStep <= 1 && book->frame < 0x28) {
                book->previewStep++;
                book->animationIndex = frames.v[book->previewStep];
                Anim_LoadKeyframe(&book->preview, &book->previewResource,
                                  (u16)(book->animationIndex + book->animationBase), 2);
                Anim_SetKeyframes(&book->preview, &book->previewResource,
                                  (u16)(book->animationIndex + book->animationBase), 2, 4);
            }
            if (book->animationIndex == 0x54)
                PlayerAttackEffect_SetEmissionState(1);
            else if (book->animationIndex == 0x72)
                PlayerAttackEffect_SetEmissionState(3);
            else if (book->animationIndex != 0x5e)
                PlayerAttackEffect_SetEmissionState(2);
        } else {
            if (book->frame == 0x28) {
                book->preview.position.x = gCamera.position.x - 456;
                book->preview.position.z = gCamera.position.z + 336;
                book->animationIndex = 0;
                book->animationBase = 3;
                book->animationFlag = 1;
                Anim_LoadKeyframe(&book->preview, &book->previewResource,
                                  (u16)(book->animationIndex + book->animationBase), 2);
                Anim_SetKeyframes(&book->preview, &book->previewResource,
                                  (u16)(book->animationIndex + book->animationBase), 2, 4);
            }
        }
    }
    book->frame++;
}
