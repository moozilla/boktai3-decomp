#include "script_prompt.h"
#include "ram.h"
#include "input.h"
#include "render.h"
#include "engine.h"

// Names are tentative, based on the static state transitions and memory writes.

void ScriptPrompt_ResetDisplay(struct ScriptPromptScreen *p)
{
    u8 text[32];
    u32 zero;
    u8 *src, *dst;
    Menu_EraseRect(4, 5, 5, 2);
    Menu_DrawText(4, 5,
                  Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 8));
    Menu_EraseRect(9, 5, 16, 2);
    Menu_DrawText(9, 5, PlayerName_GetBuffer());
    if (gScriptPromptSettings.showDescription) {
        Menu_EraseRect(3, 8, 24, 4);
        Menu_DrawText(4, 8,
                      Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 7));
        zero = 0;
        CpuSet(&zero, text, 0x05000008);
        src = Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.title));
        dst = text;
        dst[0] = 0x80;
        dst[1] = 0xb9;
        dst += 2;
        while (*src) {
            *dst = *src;
            src++;
            dst++;
        }
        dst[0] = 0x80;
        dst[1] = 0xba;
        Menu_DrawText(3, 10, text);
    }
    p->selection = 0;
    p->widgets[1].sprite.flags &= ~1;
    p->widgets[2].sprite.flags |= 1;
    p->widgets[1].sprite.position.x = -32;
    p->widgets[1].sprite.position.y = -24;
    p->widgets[2].sprite.position.x = -32;
    p->widgets[2].sprite.position.y = -24;
    p->timer = 0;
}

void ScriptPrompt_DrawFooterText(struct ScriptPromptScreen *unused)
{
    Menu_EraseRect(4, 0xd, 0x16, 2);
    Menu_DrawText(4, 0xd,
                  Text_LookupString(Script_ParseStringRef(gScriptPromptSettings.message) + 0xa));
}

void ScriptPrompt_UpdateSelection(struct ScriptPromptScreen *p)
{
    if (p->selection == 0) {
        p->widgets[1].sprite.flags &= ~1;
        p->widgets[2].sprite.flags |= 1;
    } else {
        p->widgets[1].sprite.flags |= 1;
        p->widgets[2].sprite.flags &= ~1;
    }
}

void ScriptPrompt_Update(struct ScriptPromptScreen *s)
{
    if (s->timer == 1)
        ScriptPrompt_DrawFooterText(s);
    switch (s->state) {
        case 0:
            if (gPadState[1] & 1) {
                if (s->selection == 0) {
                    Sound_PlaySong(0xDD);
                    s->fadeSpeed = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    s->result = 1;
                    s->state = 1;
                } else {
                    Sound_PlaySong(0xDE);
                    Menu_StartFadeOut(s->fadeSpeed);
                    s->result = 0;
                    s->state = 1;
                }
            } else if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                Menu_StartFadeOut(s->fadeSpeed);
                s->result = 0;
                s->state = 1;
            }
            if (((gPadState[1] & 0x20) && s->selection == 1) ||
                ((gPadState[1] & 0x10) && s->selection == 0)) {
                s->selection = 1 - s->selection;
                Sound_PlaySong(0x37C);
            }
            Palette_UpdateHighlightPulse(&s->colors[12], s->pulseState);
            ScriptPrompt_UpdateSelection(s);
            break;
        case 1:
            if (s->fadeTimer > (1 << s->fadeSpeed) || s->fadeSpeed == 0) {
                s->widgets[2].sprite.flags |= 1;
                s->widgets[1].sprite.flags |= 1;
                s->widgets[0].sprite.flags |= 1;
                if (s->result == 0)
                    Script_Call(gScriptPromptSettings.cancelScript, 0);
                else
                    Script_Call(gScriptPromptSettings.acceptScript, 0);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 2:
            break;
    }
    s->timer++;
}

void ScriptPrompt_Destroy(struct ScriptPromptScreen *p)
{
    Anim_Remove(&p->widgets[2].sprite);
    Anim_Remove(&p->widgets[1].sprite);
    Anim_Remove(&p->widgets[0].sprite);
}

s32 ScriptPrompt_Initialize(struct ScriptPromptScreen *s)
{
    u32 zero;
    u32 layer;
    u32 resourceBank;
    void *background;
    void *palette;
    void *resource;
    u16 *sourcePalette;
    struct SceneInputResource *frameResource;
    struct SceneInputResource *acceptResource;
    struct SceneInputResource *cancelResource;
    u16 **widgetPalette;
    u16 **savedPalette;
    u16 *colors;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    background = ScriptProc_Start(0xC091, 0x3536);
    layer = 5;
    Bg_SetupLayers(2, 0, background, 0, zero, 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    palette = ((struct ScriptPromptPalette *)ScriptProc_Start(0x92B3, 0x204))->colors;
    CpuSet(palette, gBgPaletteBuffer, 0x100);
    resourceBank = 0xCB05;
    resource = ScriptProc_Start(resourceBank, 0xE2AB);
    frameResource = &s->widgets[0].resource;
    ResourceHeader_Relocate(frameResource, resource);
    Widget_InitLinked(&s->widgets[0], frameResource, 0x37, 0x10, zero, zero, zero, zero);
    resource = ScriptProc_Start(resourceBank, 0x530D);
    acceptResource = &s->widgets[1].resource;
    ResourceHeader_Relocate(acceptResource, resource);
    Widget_InitLinked(&s->widgets[1], acceptResource, 0x4B, 0x10, zero, zero, zero, zero);
    cancelResource = &s->widgets[2].resource;
    ResourceHeader_Relocate(cancelResource, resource);
    Widget_InitLinked(&s->widgets[2], cancelResource, 0x4C, 0x10, zero, zero, zero, zero);
    savedPalette = &s->savedPalette;
    widgetPalette = &s->widgets[1].sprite.palette;
    sourcePalette = *widgetPalette;
    *savedPalette = sourcePalette;
    colors = s->colors;
    CpuSet(sourcePalette, colors, 0x10);
    *widgetPalette = colors;
    s->widgets[2].sprite.palette = colors;
    s->fadeSpeed = zero;
    Menu_StartFadeIn(0);
    ScriptPrompt_ResetDisplay(s);
    return 0;
}

void ScriptPrompt_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScriptPromptSettings.message = Script_GetPc();
    if (Script_SeekToKeyword('t'))
        gScriptPromptSettings.title = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gScriptPromptSettings.acceptScript = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScriptPromptSettings.cancelScript = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gScriptPromptSettings.showDescription = Script_GetValue();
}

void *ScriptPrompt_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct ScriptPromptScreen));
    if (r != 0) {
        Task_SetCallbacks(r, ScriptPrompt_Update, ScriptPrompt_Destroy);
        if (ScriptPrompt_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
