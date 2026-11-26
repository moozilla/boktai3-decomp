#include "ui/widgets/scene_input_panel.h"
#include "engine/video/display_sequence_inline.h"

static inline void SceneInputPanel_StepDrawable(struct SceneInputPanelDrawable *drawable,
                                                struct DisplaySequenceState *a)
{
    const u16 *frame;
    frame = a->commands + a->index;
    drawable->frame = *frame >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            drawable->flags |= 4;
        else
            drawable->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*frame & 0x30) >> 4) & bit))
            drawable->flags |= 8;
        else
            drawable->flags &= ~8;
    }
    if (++a->elapsed >= a->duration) {
        DisplaySequence_SelectNext(a);
        frame = a->commands + a->index;
        DisplaySequence_UpdateWeight(a, frame);
        if (!a->duration)
            a->duration = 1;
    }
}

void SceneInputPanel_InitWidgets(struct SceneInputPanel *owner, s32 mode)
{
    struct SceneInputPanel *panel = owner;
    struct SceneInputPackedPosition position;
    struct DisplaySequenceState sequence;
    struct SceneInputResource *source;
    void *sequenceResource;
    void *graphics;
    s32 i, k;
    struct SceneWidgetPlayback playback;
    u8 priority;

    source = ScriptProc_Start(0xCB05, 0x5D04);
    panel->gridResource = *source;
    ResourceHeader_Relocate(&panel->gridResource, source);
    source = ScriptProc_Start(0xCB05, 0x530D);
    panel->widgetResource = *source;
    ResourceHeader_Relocate(&panel->widgetResource, source);
    playback.priority = 0;
    playback.mode = 0;
    position.x = 0;
    position.y = 0;
    position.z = 0;
    priority = playback.priority;
    Widget_InitLinked(&panel->indicator, &panel->widgetResource, gUnk_0824DEEA[mode], 0x30,
                      priority, priority, 60, &position);

    sequenceResource = ScriptProc_Start(0x922E, 0x4BBF);
    graphics = panel->drawable.graphics;
    AnimData_Load(graphics, gUnk_0824DF00[mode]);
    AnimData_SelectPaletteBank(graphics, gUnk_0824DF14[mode]);
    Sprite_Init(&panel->drawable, graphics, 0x30);
    Anim_RestartSequence(&sequence, sequenceResource, gUnk_0824DF1E[mode], 0, 0);
    SceneInputPanel_StepDrawable(&panel->drawable, &sequence);
    panel->drawable.priority = 0;
    panel->drawable.position.x = 8;
    panel->drawable.position.y = 80;

    Widget_InitLinked(&panel->mode, &panel->widgetResource, gUnk_0824DEF4[mode], 0x30, 0, 0, 60,
                      &position);
    Widget_InitLinked(&panel->valueA, &panel->widgetResource, 0x41, 0x31, 0, 0, 60, &position);
    position.x = 8;
    Widget_InitLinked(&panel->valueB, &panel->widgetResource, 0x42, 0x31, 0, 0, 60, &position);
    for (i = 0; i <= 15; i++) {
        position.x = (i & 3) * 24 + 112;
        position.y = (i >> 2) * 24 + 32;
        Widget_InitLinked(&panel->primary[i], &panel->gridResource, 0x151, 0x31, 0, 0, 60,
                          &position);
    }
    for (i = 0; i <= 7; i++) {
        position.x = (i & 3) * 24 + 8;
        position.y = (i >> 2) * 24 + 32;
        Widget_InitLinked(&panel->secondary[i], &panel->gridResource, 0x151, 0x31, 0, 0, 60,
                          &position);
    }
    position.x = 0;
    position.y = 128;
    Widget_InitLinked(&panel->banner, &panel->gridResource, 0x189, 0x31, 0, 0, 60, &position);
    position.x = 216;
    position.y = 56;
    Widget_InitLinked(&panel->buttons[1], &panel->gridResource, 0x192, 0x30, 0, 0, 60, &position);
    position.x = 216;
    position.y = 80;
    Widget_InitLinked(&panel->buttons[2], &panel->gridResource, 0x191, 0x30, 0, 0, 60, &position);
    position.x = 216;
    position.y = 32;
    Widget_InitLinked(&panel->buttons[0], &panel->gridResource, 0x18F, 0x30, 0, 0, 60, &position);
    position.x = 216;
    position.y = 104;
    Widget_InitLinked(&panel->buttons[3], &panel->gridResource, 0x190, 0x30, 0, 0, 60, &position);
    Widget_InitLinked(&panel->category, &panel->widgetResource, 0x4E, 0x31, 0, 0, 60, &position);
    position.x = 112;
    position.y = 32;
    Widget_InitLinked(&panel->compareA, &panel->widgetResource, 0x4D, 0x31, 0, 0, 60, &position);
    position.x = 216;
    position.y = 32;
    Widget_InitLinked(&panel->modeCursor, &panel->widgetResource, 0x4D, 0x30, 0, 0, 60, &position);
    position.x = 0;
    position.y = 0;
    Widget_InitLinked(&panel->heading, &panel->widgetResource, gUnk_0824DEE0[mode], 0x30, 0, 0, 60,
                      &position);
    position.x = 8;
    position.y = 0;
    Widget_InitLinked(&panel->valueLabel, &panel->widgetResource, 0x40, 0x30, 0, 0, 60, &position);
    position.x = 0;
    position.y = 0;
    Widget_InitLinked(&panel->glyph, &panel->widgetResource, 0x67, 0x30, 0, 0, 60, &position);
    position.x = 56;
    position.y = 0;
    Widget_InitLinked(&panel->meterGlyph, &panel->widgetResource, 0x68, 0x30, 0, 0, 60, &position);
    for (i = 0; i < 8; i++)
        Widget_InitLinked(&panel->digits[i], &panel->widgetResource, 0x50, 0x30, 0, 0, 60,
                          &position);
    Widget_InitLinked(&panel->numericLabel, &panel->widgetResource, 0xA9, 0x31, 0, 0, 60,
                      &position);
    position.x -= 11;
    for (i = 0; i < 4; i++) {
        Widget_InitLinked(&panel->smallDigits[i], &panel->widgetResource, 0xAA, 0x31, 0, 0, 60,
                          &position);
        position.x += 6;
    }
    position.x = 0;
    position.y = 0;
    Widget_InitLinked(&panel->prompt, &panel->widgetResource, 0x4B, 0x11, 0, 0, 60, &position);
}
