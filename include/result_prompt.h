#ifndef RESULT_PROMPT_H
#define RESULT_PROMPT_H
#include "global.h"
#include "engine.h"

// Tentative: a timed text prompt. It shows a message window, waits 60 frames, asks sub_0822BE20
// for a result (1 selects the first text, otherwise the second), then closes after A is pressed
// (from frame 120) or at frame 300 and calls the script from keyword 'e' with the result.
struct ResultPrompt {
    u8 task[24];
    s32 window;        // text window id, or -1
    u32 script;        // script to call when done (keyword 'e'), cleared once called
    s32 timer;         // frames since creation
    s32 result;        // -1 until the result is known
    const u8 *message; // text script for the window (keyword 's')
};

struct ResultPromptArgs {
    u32 count : 16;
    u32 pad : 16;
    s32 *values;
};

extern s32 gUnk_03005408;
extern u16 gUnk_03005260[]; // [1] = newly pressed keys

u8 sub_0822BE20(void);
void sub_08177DEC(s32);
void sub_08033778(s32);
s32 sub_08033690(u32, u32, u32, u32);
void sub_080337FC(s32);
u32 sub_08227E90(void);

int ResultPrompt_Update(struct ResultPrompt *);
u32 sub_0800F914(struct ResultPrompt *);
s32 sub_0800F924(struct ResultPrompt *);
u8 *sub_0800F988(void);

#endif
