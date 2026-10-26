#include "global.h"
#include "game_state.h"
#include "ram.h"
struct State {
    u8 pad0[7];
    u8 pending, entered, mode, skip;
    u8 padb[4];
    u8 fresh;
    u8 pad10[5];
    s8 current;
    u8 instant;
    u8 pad17[5];
    void *pc;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pada, rate, timer;
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad38[0x144];
    u8 x, y, width, height;
    u16 scriptIndex;
    u8 mode, pad183, previousMode, pad185, scrollMode, defaultMode;
    u16 scrollStep, scrollCounter;
    void *custom;
    u32 customCount;
    u16 palette, wait;
    u32 scrollFlags;
    u32 counter;
    u8 pad1a0[4];
    void *script;
    u16 scriptFrame, scriptCount;
    u8 pad1ac[0x6c];
    u32 flags;
    u8 pad21c[0x54];
    u8 sprite[0x1c];
    struct Render render;
    u8 pad2a0[0x18];
    struct Frames frames;
    void *resource;
    void *callback;
};
int MessageWindow_UpdateSpeakerPortrait(struct Actor *);
void sub_080316A4(void);
void MessageWindow_UpdateImmediatePlayback(void);
void MessageWindow_ClearTextCompletion(void);
int TextWindow_BlankRect(int, int, int, int);
void *MessageWindow_GetStringAtOffset(struct Actor *, int);
void TextState_Reset(struct State *);
int TextState_SetBounds(struct State *, int, int, int, int);
int Script_GetValue(void);
static inline void setCallback(void *fn, struct Actor *p)
{
    p->callback = fn;
}
static inline void setCallbackB(struct Actor *p, void *fn)
{
    p->callback = fn;
}
struct Vm {
    u8 pad0[9];
    u8 mode;
    u8 pada[3];
    u8 choiceCount;
    u8 pade[3];
    u8 spacing;
    u8 pad12[0xb6];
    u32 choices[16];
};
struct Sun {
    u8 pad[0x12];
    s16 level;
};
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void *Script_GetPc(void);
int Script_GetKeywordValue(int, int);
int MessageWindow_LoadPalette(struct Actor *, int);
void MessageWindow_ApplyPalette(struct Actor *);
int MessageWindow_SetMessageTable(void *);
int MessageWindow_SetBounds(int, int, int, int);
int TextState_StepPlayback(struct State *);
void MessageWindow_UpdateIdle(void);
void sub_080313E4(void);
extern u16 gUnk_03005260[];
void Anim_Remove(void *);
void Sprite_Remove(void *);
int MessageWindow_OnDestroy(struct Actor *p)
{
    Anim_Remove((u8 *)p + 0x210);
    Sprite_Remove(&p->render);
    return (gMessageWindow = 0);
}
