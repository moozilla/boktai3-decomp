#include "global.h"
#include "engine.h"
#include "game_state.h"
struct State {
    u8 pad0[9];
    u8 lineWidth;
    u8 pad0a[3];
    u8 valueCount;
    u8 pad0e[3];
    u8 spacing;
    u8 pad12[0xb6];
    u32 values[1];
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad[0x180 - 0x18 - sizeof(struct State)];
    u16 scriptIndex;
    u8 mode, flag183, previousMode, pad185, scrollMode, defaultMode;
    u16 scrollStep, scrollCounter;
    u8 pad18c[8];
    u16 palette, wait;
    u32 scrollFlags;
    u8 pad19c[8];
    void *script;
};
extern struct Actor *gUnk_020000DC;
void *sub_08227E90(void);
void sub_08030C84(void *);
int sub_08030F20(int x, int y, int width, int height);
int sub_080308D0(struct Actor *, int);
void sub_08030908(struct Actor *);
int MessageWindow_OpenFromScript(void)
{
    struct Actor *p = gUnk_020000DC;
    struct State *state;
    if (!p)
        return -1;
    state = &p->state;
    if (Script_SeekToKeyword('s')) {
        void *script = sub_08227E90();
        p->script = script;
        if (!script)
            return -1;
        sub_08030C84(script);
    }
    if (Script_SeekToKeyword('r')) {
        int x = Script_GetValue();
        int y = Script_GetValue();
        int width = Script_GetValue();
        int height = Script_GetValue();
        sub_08030F20(x, y, width, height);
    }
    {
        s32 palette = Script_GetKeywordValue('c', p->palette);
        if (sub_080308D0(p, palette) < 0)
            return -1;
    }
    sub_08030908(p);
    p->scriptIndex = Script_GetKeywordValue('i', p->scriptIndex);
    state->spacing = Script_GetKeywordValue('m', state->spacing);
    state->lineWidth = Script_GetKeywordValue('w', (s16)gUnk_02000710->unk12);
    if (Script_SeekToKeyword('d')) {
        p->scrollMode = Script_GetValue();
        p->scrollStep = Script_GetValue();
        p->scrollFlags = 1;
    } else {
        p->scrollMode = 0;
        p->scrollStep = 0;
        p->scrollFlags = 0;
    }
    p->scrollCounter = 0;
    if (Script_SeekToKeyword('p')) {
        state->valueCount = 0;
        while (Script_GetPc()) {
            state->values[state->valueCount] = Script_GetValue();
            state->valueCount++;
        }
    }
    if (Script_SeekToKeyword('a')) {
        if (Script_GetValue())
            p->flag183 = 0;
    } else
        p->flag183 = 0;
}
