#include "global.h"
#include "game_state.h"
#include "ram.h"

/* NameEntry_Update: per-frame task callback. State 0 handles the d-pad (with key repeat),
 * B (erase, or cancel on an empty name), SELECT (page toggle), START (jump to End) and A
 * (type / toggle / confirm); confirming stores the name and fades out. State 1 waits for
 * the fade, hides the widgets, runs the follow-up script and kills the task. (guess) */

struct NameEntryTask {
    u8 widgets[0x348];
    s32 unk348;
    s32 selection; /* 0x34C: keyboard key; 0x55 page toggle, 0x56 End */
    s32 position;  /* 0x350: cursor position in the name */
    void *bg;
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
    u32 slot;      /* 0x370 */
    u8 name[0x10]; /* 0x374 */
};

void TextEntry_DrawName(u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
s32 sub_081DDB44(struct NameEntryTask *p)
{
    u8 *ch = &p->name[p->position * 2];
    u8 *placeholder =
        Text_LookupString(Script_ParseStringRef((const void *)gNamePlaceholderScript[0]) + 0x54);
    s32 result = 0;
    if ((ch[0] == 0 && ch[1] == 0) || (ch[0] == placeholder[0] && ch[1] == placeholder[1])) {
        if (p->position == 0) {
            result = 1;
            goto draw;
        }
        p->position--;
        ch = &p->name[p->position * 2];
    }
    ch[0] = 0;
    ch[1] = 0;
draw:
    TextEntry_DrawName(p->name);
    return result;
}
