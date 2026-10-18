#define SelectionPanel_RunSelectionScript sub_0810E284
#define SelectionPanel_GetSelection sub_0810E1A0
#include "global.h"
#include "directional_interaction.h"
struct SelectionPanel {
    u8 unknown0[0xCE4];
    void (*update)(void);
    u8 unknownCE8[0x1C];
    u8 state;
    u8 unknownD05[0x27];
    u8 entered;
    u8 unknownD2D[7];
    u32 selectionScript;
};
void Bg_ClearTilemap(s32);
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Registry3C9F_HideDisplayObjects(struct SelectionPanel *);
s32 sub_0810E1A0(struct SelectionPanel *);
void Registry3C9F_OnTransitionIdle(void);
void sub_0810E284(struct SelectionPanel *p)
{
    s32 value;
    struct DirectionalScriptArgs args;
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    Registry3C9F_HideDisplayObjects(p);
    if (p->selectionScript) {
        value = sub_0810E1A0(p);
        args.count = 1;
        args.values = &value;
        Script_Call(p->selectionScript, &args);
        {
            void (*update)(void) = Registry3C9F_OnTransitionIdle;
            u32 state = 3;
            *(void (**)(void))((u8 *)p + 0xCE4) = update;
            p->entered = 1;
            p->state = state;
        }
    }
}
