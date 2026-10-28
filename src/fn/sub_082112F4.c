#include "global.h"
#include "input.h"

struct Keys {
    u16 held;
    u16 pressed;
};

void Sound_PlaySong(u32);
u32 CollectionMenu_IsValidGridPosition(s32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Collection_RefreshPage(void *);
void Collection_RefreshSelection(void *);
void Collection_UpdateHighlightPulse(void *);
void Script_Call(void *, u32);
void Task_Kill(void *);

#define PAGE        (*(s16 *)(m + 0xb80))
#define COL         (*(s16 *)(m + 0xb84))
#define ROW         (*(s16 *)(m + 0xb86))
#define INDEX       (*(u16 *)(m + 0xb88))
#define EXITING     (*(s16 *)(m + 0xb8c))
#define FADE        (*(s16 *)(m + 0xb8e))
#define TIMER       (*(s16 *)(m + 0xb90))
#define EXIT_SCRIPT (*(void **)(m + 0xba4))

/* Tentative: L/R change page, d-pad moves the cursor, B closes the menu.
 * Declared s32 with no return statement: the original leaves r0 live. */
s32 CollectionMenu_Update(u8 *m)
{
    u32 pageChanged = 0;
    u32 moved = 0;

    if (EXITING == 0) {
        if ((*(struct Keys *)gPadState).pressed & 0x200) {
            if (--PAGE < 0)
                PAGE = 2;
            COL = moved;
            ROW = moved;
            Sound_PlaySong(0x10a);
            pageChanged = 1;
        } else if ((*(struct Keys *)gPadState).pressed & 0x100) {
            if (++PAGE > 2)
                PAGE = moved;
            COL = moved;
            ROW = moved;
            Sound_PlaySong(0x10a);
            pageChanged = 1;
        }
        if (((*(struct Keys *)gPadState).pressed & 0x40) &&
            (CollectionMenu_IsValidGridPosition(COL, ROW - 1, PAGE))) {
            ROW--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if (((*(struct Keys *)gPadState).pressed & 0x80) &&
                   (CollectionMenu_IsValidGridPosition(COL, ROW + 1, PAGE))) {
            ROW++;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if (((*(struct Keys *)gPadState).pressed & 0x20) &&
                   (CollectionMenu_IsValidGridPosition(COL - 1, ROW, PAGE))) {
            COL--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if (((*(struct Keys *)gPadState).pressed & 0x10) &&
                   (CollectionMenu_IsValidGridPosition(COL + 1, ROW, PAGE))) {
            COL++;
            moved = 1;
            Sound_PlaySong(0xdc);
        }
        if ((*(struct Keys *)gPadState).pressed & 2) {
            ScreenTransition_SendCommand(1, FADE, 4, 4, 4, 0xffff, 0);
            EXITING = 1;
            Sound_PlaySong(0x119);
        }
    }
    if (pageChanged || moved) {
        u16 idx = PAGE * 10 + ROW * 5 + COL;
        INDEX = idx;
    }
    if (pageChanged)
        Collection_RefreshPage(m);
    if (moved)
        Collection_RefreshSelection(m);
    if (EXITING) {
        if (TIMER > 1 << FADE) {
            if (EXIT_SCRIPT)
                Script_Call(EXIT_SCRIPT, 0);
            Task_Kill(m);
        }
        TIMER++;
    } else {
        Collection_UpdateHighlightPulse(m);
    }
}
