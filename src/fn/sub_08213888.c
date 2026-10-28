#include "global.h"

struct Keys {
    u16 held;
    u16 pressed;
};
struct ScriptArg {
    u16 type;
    void *value;
};
extern struct Keys gPadState;

void Sound_PlaySong(u32);
u32 ForgingBook_IsGridPositionValid(s32, s32, s32);
s32 CollectionBook_IsEntryCollected(s32);
void CollectionBook_UpdateTrackedEntryMarker(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void ForgingBook_RefreshPage(void *);
void sub_0821353C(void *);
void ForgingBook_UpdateCursorPulse(void *);
void Script_Call(void *, void *);
void Task_Kill(void *);

#define IDS         ((s16 *)(m + 0x6e0))
#define PAGE        (*(s16 *)(m + 0x6f4))
#define COL         (*(s16 *)(m + 0x6f8))
#define ROW         (*(s16 *)(m + 0x6fa))
#define INDEX       (*(s16 *)(m + 0x6fc))
#define EXITING     (*(s16 *)(m + 0x700))
#define FADE        (*(s16 *)(m + 0x702))
#define TIMER       (*(s16 *)(m + 0x704))
#define LOCKED      (*(s16 *)(m + 0x706))
#define EXIT_SCRIPT (*(void **)(m + 0x718))
#define CLOSING     (*(s16 *)(m + 0x71c))
#define BUSY        (*(s16 *)(m + 0x71e))
#define TRACKED     (*(s16 *)(m + 0x722))

// confidence=low: handles collection-grid selection and the menu close transition.
#define CollectionBook_UpdateMenu sub_08213888
s32 CollectionBook_UpdateMenu(u8 *m)
{
    u32 pageChanged = 0;
    u32 moved = 0;
    s32 val[2];
    struct ScriptArg arg;

    if (EXITING == 0 && BUSY == 0) {
        if (gPadState.pressed & 2) {
            ScreenTransition_SendCommand(1, FADE, 4, 4, 4, 0xffff, moved);
            EXITING = 1;
            CLOSING = 1;
            Sound_PlaySong(0x119);
        } else if (gPadState.pressed & 1) {
            s16 *ip = &INDEX;
            s32 o = *ip * 2;
            u8 *ids = m + 0x6e0;
            if (CollectionBook_IsEntryCollected(*(s16 *)(ids + o)) && LOCKED == 0) {
                Sound_PlaySong(0xdd);
                {
                    s32 off = *ip * 2;
                    s16 t = *(s16 *)(ids + off);
                    TRACKED = t;
                }
                CollectionBook_UpdateTrackedEntryMarker(m);
            } else {
                Sound_PlaySong(0x192);
            }
        } else if ((gPadState.pressed & 0x40) &&
                   ForgingBook_IsGridPositionValid(COL, ROW - 1, PAGE)) {
            ROW--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState.pressed & 0x80) &&
                   ForgingBook_IsGridPositionValid(COL, ROW + 1, PAGE)) {
            ROW++;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState.pressed & 0x20) &&
                   ForgingBook_IsGridPositionValid(COL - 1, ROW, PAGE)) {
            COL--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState.pressed & 0x10) &&
                   ForgingBook_IsGridPositionValid(COL + 1, ROW, PAGE)) {
            COL++;
            moved = 1;
            Sound_PlaySong(0xdc);
        }
    }
    if (pageChanged || moved) {
        u16 idx = PAGE * 10 + ROW * 5 + COL;
        INDEX = idx;
    }
    if (pageChanged)
        ForgingBook_RefreshPage(m);
    if (moved)
        sub_0821353C(m);
    if (EXITING) {
        if (TIMER > 1 << FADE) {
            if (EXIT_SCRIPT) {
                val[0] = TRACKED;
                arg.type = 2;
                arg.value = val;
                Script_Call(EXIT_SCRIPT, &arg);
            }
            Task_Kill(m);
        }
        TIMER++;
    } else {
        ForgingBook_UpdateCursorPulse(m);
    }
}
