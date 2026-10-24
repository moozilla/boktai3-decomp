#include "selection_menu.h"
#define Presentation_PlaySelectionSound sub_0801A0D0
#define SelectionMenu_HideMarkers       sub_0801A614

void ActionRecord_CopyFromEntry(u8 *, void *);
void Presentation_ApplyGraphicData(void *, void *);
void Presentation_ApplyGraphicSettings(u8 *, u32);
u32 Presentation_SelectGraphicResource(u8 *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_SetNumberArgument(u32, u32);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void MessageWindow_SetBounds(int, int, int, int);
void sub_0801B600(int, int, void *);
void SceneHud_DrawClock(int);
void SceneHud_DrawNumericStatus(int);
void Text_DrawNumber(int, int, int, int, int, int, int);
void SceneInputPanel_GetEquippedValues(int *, int *, int, int);
void SceneHud_FillTile020(int, int, int);
int Player_GetCurrentAttribute(int);
int GameState_IsConsumableSlotEquipped(int);
void MessageWindow_SetDisplayMode(int);
void MessageWindow_SetPalette(int);
void Widget_CopyMasked(void *, void *, int);
void Vec16_Lerp(void *, void *, void *, u32, int);
int Inventory_HasItem(int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void ScrollingDialogue_RequestHide(void);
void ScrollingDialogue_RequestShow(void);

u8 *Inventory_GetEquipmentEntry(u32);
int Inventory_GetConsumableId(u32);
void ScrollingDialogue_SetScript(u32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_ShowMessage(u32);
void sub_0801A218(struct SelectionMenu *);
void SelectionMenu_InitCursors(struct SelectionMenu *);
void SelectionMenu_InitMarkers(struct SelectionMenu *);
void sub_0801A6D0(struct SelectionMenu *);
int SelectionMenu_NavigateGrid(struct SelectionMenu *);
extern u8 gUnk_03005160[];
u32 Script_GetTextSourcePc(void);

u8 *SelectionMenu_GetIconPalette(s32 i);
void Presentation_SetActorIconFrame(struct SelectionMenu *p, struct SceneInputWidget *q, u8 *r);
void SelectionMenu_SetActorIcon(struct SelectionMenu *p, struct SceneInputWidget *q, u32 c);
void Presentation_SetItemGraphic(struct SelectionMenu *p, struct SceneInputWidget *q, u32 item);
void SelectionMenu_SetCallback(struct SelectionMenu *p, void (*v)(struct SelectionMenu *));
u32 SelectionMenu_ConsumeCallbackFlag(struct SelectionMenu *p);
void Presentation_UpdateActorGraphic(struct SelectionMenu *p, u32 fallback);
void Presentation_ShowItemDescription(u8 *p, u32 q);
void Presentation_UpdateItemGraphic(struct SelectionMenu *p, u32 fallback);
void SelectionMenu_CallConfirmScript(struct SelectionMenu *p);
void Presentation_PlaySelectionSound(void *p, int selection);
int SelectionMenu_ConfigureLayers(int value, int mode);
void SelectionMenu_HandleHorizontalInput(struct SelectionMenu *p);
void SelectionMenu_InitHighlight(struct SelectionMenu *p);
void SelectionMenu_PositionCursors(struct SelectionMenu *p);
void SelectionMenu_PositionInventoryMarkers(struct SelectionMenu *p);
void SelectionMenu_PositionEquipmentMarkers(struct SelectionMenu *p);
void SelectionMenu_HideMarkers(struct SelectionMenu *p);
void SelectionMenu_StepCursors(struct SelectionMenu *p);
void SelectionMenu_RemovePanel(struct SelectionMenu *p);
void SelectionMenu_RemoveCursors(struct SelectionMenu *p);
void SelectionMenu_RemoveMarkerAnim(struct SelectionMenu *p);
void SelectionMenu_RemoveHighlight(struct SelectionMenu *p);
void SelectionMenu_PopulateActors(struct SelectionMenu *p);
void SelectionMenu_PopulateItems(struct SelectionMenu *p);
void SelectionMenu_RemoveIconGrid(struct SelectionMenu *p);
void SelectionMenu_StartAnimateChoice(struct SelectionMenu *p);
void SelectionMenu_Back(struct SelectionMenu *p);
void SelectionMenu_AdvanceChoice(struct SelectionMenu *p);
void SelectionMenu_DrawStatistics(struct SelectionMenu *p);
void SelectionMenu_ActorAvailability(struct SelectionMenu *p);
void SelectionMenu_ItemAvailability(struct SelectionMenu *arg);
void SelectionMenu_OpenActorPage(struct SelectionMenu *p);
void SelectionMenu_RefreshActorPage(struct SelectionMenu *p);
void SelectionMenu_RefreshItemPage(struct SelectionMenu *p);
void SelectionMenu_Choose(struct SelectionMenu *p);
void SelectionMenu_AnimateChoice(struct SelectionMenu *p);
void SelectionMenu_ReviewChoice(struct SelectionMenu *p);
void SelectionMenu_Close(struct SelectionMenu *p);
void SelectionMenu_Hide(struct SelectionMenu *arg);
void SelectionMenu_Confirm(struct SelectionMenu *p);
u32 SelectionMenu_Update(struct SelectionMenu *p);
u32 SelectionMenu_Destroy(struct SelectionMenu *p);
int SelectionMenu_Initialize(struct SelectionMenu *p, void *a, void *b);
void *SelectionMenu_Create(void *a, void *b);

static inline void SelectionMenu_ClearPosition(void *source, void *destination)
{
    CpuSet(source, destination, 0x05000002);
}

// CpuFill32(0, position, 8), but the destination is evaluated before the zero is stored.
static inline void Position_Clear(struct ContactPosition *position)
{
    vu32 zero = 0;
    CpuSet((void *)&zero, position, 0x05000002);
}

u8 *SelectionMenu_GetIconPalette(s32 i)
{
    return gUnk_030042E4 + (i + 0x373) * 0x20;
}

void Presentation_SetActorIconFrame(struct SelectionMenu *p, struct SceneInputWidget *q, u8 *r)
{
    Anim_LoadKeyframe(q, &p->iconResource, *r + 0xf0, 0);
    q->flags &= ~1;
}

void SelectionMenu_SetActorIcon(struct SelectionMenu *p, struct SceneInputWidget *q, u32 c)
{
    u8 *r = Inventory_GetEquipmentEntry(c);
    if (*r != 0xff) {
        Presentation_SetActorIconFrame(p, q, r);
        q->flags &= ~1;
    } else {
        q->flags |= 1;
    }
}

// MATCHING_CONCESSION: register-pinning
void Presentation_SetItemGraphic(struct SelectionMenu *p, struct SceneInputWidget *q, u32 item)
{
    int index = Inventory_GetConsumableId(item);
    if (index >= 0) {
        int offset = index * 16;
        u8 *entry;
        entry = (u8 *)((u32)gUnk_08E60F80 + offset);
        {
            void *context = &p->iconResource;
            register int base asm("r0");
            int resource;
            base = 0x121;
            asm("" : "+r"(base));
            resource = base;
            asm("" : "+r"(resource));
            {
                register int tile asm("r3") = entry[0];
                resource += tile;
            }
            Anim_LoadKeyframe(q, context, resource, 0);
        }
        q->flags &= ~1;
    } else
        q->flags |= 1;
}

void SelectionMenu_SetCallback(struct SelectionMenu *p, void (*v)(struct SelectionMenu *))
{
    p->frame = 0;
    p->callbackPending = 1;
    p->callback = v;
}

u32 SelectionMenu_ConsumeCallbackFlag(struct SelectionMenu *p)
{
    u32 r;
    if (p->callbackPending == 0) {
        r = 0;
    } else {
        p->callbackPending = 0;
        r = 1;
    }
    return r;
}

void Presentation_UpdateActorGraphic(struct SelectionMenu *p, u32 fallback)
{
    u8 *actor = Inventory_GetEquipmentEntry(p->selectedSlot);
    u8 data[32];
    if (*actor != 255) {
        ActionRecord_CopyFromEntry(actor, data);
        MessageWindow_SetMessageTable(p->actorMessages);
        Presentation_ApplyGraphicData(data, p->graphicData);
        Presentation_ApplyGraphicSettings(actor, p->actorMessages);
        {
            u32 resource = Presentation_SelectGraphicResource(actor);
            (&p->highlight)->flags &= ~1;
            Anim_LoadKeyframe(&p->highlight, &p->iconResource, (u16)resource, 0);
        }
    } else {
        MessageWindow_Clear();
        if (!fallback)
            ScrollingDialogue_ClearScript();
        else
            ScrollingDialogue_SetScript(fallback);
        (&p->highlight)->flags |= 1;
    }
}

void Presentation_ShowItemDescription(u8 *p, u32 q)
{
    u32 s = Script_ParseStringRef((void *)q) + 0x30;
    MessageWindow_SetMessageTable(q);
    MessageWindow_ShowMessage(*p);
    ScrollingDialogue_SetNumberArgument(0, 0);
    ScrollingDialogue_SetNumberArgument(1, 999);
    ScrollingDialogue_SetScript((u32)Text_LookupString(s + *p));
}

void Presentation_UpdateItemGraphic(struct SelectionMenu *p, u32 fallback)
{
    int index = Inventory_GetConsumableId(p->selectedSlot);
    if (index >= 0) {
        Presentation_ShowItemDescription(&gUnk_08E60F80[index], p->itemMessages);
        (&p->highlight)->flags &= ~1;
        Anim_LoadKeyframe(&p->highlight, &p->iconResource, 0x18a, 0);
    } else {
        MessageWindow_Clear();
        if (!fallback)
            ScrollingDialogue_ClearScript();
        else
            ScrollingDialogue_SetScript(fallback);
        (&p->highlight)->flags |= 1;
    }
}

void SelectionMenu_CallConfirmScript(struct SelectionMenu *p)
{
    struct SelectionMenuScriptValues a;
    struct SelectionMenuScriptCall b;
    b.flags = 2;
    b.args = &a;
    a.actor = p->choices[0];
    a.item = p->choices[1];
    Script_Call(p->confirmScript, &b);
}

/* Tentative: choose one of three selection sounds. */

void Presentation_PlaySelectionSound(void *p, int selection)
{
    switch (selection) {
        case 0:
            Sound_PlaySong(0x394);
            break;
        case 1:
            Sound_PlaySong(0x395);
            break;
        case 2:
            Sound_PlaySong(0x396);
            break;
    }
}

int SelectionMenu_ConfigureLayers(int value, int mode)
{
    u8 *resource = ScriptProc_Start(0xc091, 0xa597);
    int result;
    if (mode) {
        Bg_SetupLayers(3, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0);
        result = 1;
    } else {
        Bg_SetupLayers(0, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        result = 0;
    }
    {
        u8 *palette = ScriptProc_Start(0x92b3, 0x81b);
        CpuSet(palette + 0x1b4, gUnk_03005160, 0x30);
    }
    return result;
}

void SelectionMenu_HandleHorizontalInput(struct SelectionMenu *p)
{
    if ((gUnk_03005260[1] & 0x20) && p->horizontalChoice == 1) {
        p->horizontalChoice = 0;
        Anim_LoadKeyframe(&p->cursors[2], &p->panelResource, 0x4b, 0);
        Sound_PlaySong(0xdc);
    }
    if ((gUnk_03005260[1] & 0x10) && p->horizontalChoice == 0) {
        p->horizontalChoice = 1;
        Anim_LoadKeyframe(&p->cursors[2], &p->panelResource, 0x4c, 0);
        Sound_PlaySong(0xdc);
    }
}

INCLUDE_ASM("asm/nonmatching", sub_0801A218);

void SelectionMenu_InitCursors(struct SelectionMenu *p)
{
    struct {
        u32 zero;
        struct ContactPosition position;
    } scratch;
    struct SceneInputWidget *g;
    int i;
    struct ContactPosition *pos = &scratch.position;
    struct ContactPosition *saved;
    int clear;
    scratch.zero = 0;
    SelectionMenu_ClearPosition(&scratch.zero, pos);
    saved = pos;
    clear = 0;
    g = p->cursors;
    i = 2;
    do {
        Widget_InitLinked(g, &p->panelResource, 0x4e, 0x10, clear, clear, 0x3c, saved);
        g++;
        i--;
    } while (i >= 0);
}

void SelectionMenu_InitMarkers(struct SelectionMenu *p)
{
    struct {
        u32 zero;
        struct ContactPosition position;
    } scratch;
    struct SceneInputWidget *g;
    int i;
    struct ContactPosition *pos = &scratch.position;
    struct ContactPosition *saved;
    int clear;
    scratch.zero = 0;
    SelectionMenu_ClearPosition(&scratch.zero, pos);
    saved = pos;
    clear = 0;
    g = p->markers;
    i = 7;
    do {
        Widget_InitLinked(g, &p->panelResource, 0x50, 0x31, clear, clear, 0x3c, saved);
        g++;
        i--;
    } while (i >= 0);
}

void SelectionMenu_InitHighlight(struct SelectionMenu *p)
{
    struct ContactPosition z;
    u32 m = 0xFFFF0000;
    *(u32 *)&z = 0x800000;
    *((u32 *)&z + 1) &= m;
    Widget_InitLinked(&p->highlight, &p->iconResource, 0x189, 0x11, 0, 0, 0x3c, &z);
}

void SelectionMenu_PositionCursors(struct SelectionMenu *p)
{
    struct SceneInputWidget *g = &p->cursors[0];
    g->position.x = (p->selectedSlot & 3) * 24 + 144;
    g->position.y = (p->selectedSlot >> 2) * 24 + 32;
    g->flags &= ~1;
    Anim_LoadKeyframe(g, &p->panelResource, 0x4d, 0);
    g = &p->cursors[1];
    g->position.x = 16;
    g->position.y = 40;
    g->flags &= ~1;
    Anim_LoadKeyframe(g, &p->panelResource, 0x4e, 0);
    g = &p->cursors[2];
    g->position.x = 0;
    g->position.y = 0;
    g->flags |= 1;
    Anim_LoadKeyframe(g, &p->panelResource, 0x4b, 0);
}

void SelectionMenu_PositionInventoryMarkers(struct SelectionMenu *p)
{
    int i, index;
    for (i = 0; i < 8; i++) {
        struct SceneInputWidget *g = &p->markers[i];
        /* Byte-index the owner first to preserve the original address-add order. */
        if (i <= 7)
            index = ((struct GameState *)((u8 *)gUnk_02000710 + i * 2))->inventory[0];
        else
            index = -1;
        if (index >= 0) {
            g->position.x = (index & 3) * 24 + 144;
            g->position.y = (index >> 2) * 24 + 32;
            g->flags &= ~1;
        } else
            g->flags |= 1;
    }
}

void SelectionMenu_PositionEquipmentMarkers(struct SelectionMenu *p)
{
    int i, index;
    for (i = 0; i < 8; i++) {
        struct SceneInputWidget *g = &p->markers[i];
        /* Same address-order adaptation as the inventory marker loop. */
        if (i <= 3)
            index = ((struct GameState *)((u8 *)gUnk_02000710 + i * 2))->party[0];
        else
            index = -1;
        if (index >= 0) {
            g->position.x = (index & 3) * 24 + 144;
            g->position.y = (index >> 2) * 24 + 32;
            g->flags &= ~1;
        } else
            g->flags |= 1;
    }
}

/* Hide the eight inventory/equipment markers. */

void SelectionMenu_HideMarkers(struct SelectionMenu *p)
{
    int mask = 1;
    u32 *flags;
    int i;
    flags = &p->markers[0].flags;
    i = 7;
    do {
        *flags |= mask;
        flags += sizeof(struct SceneInputWidget) / sizeof(u32);
        i--;
    } while (i >= 0);
}

void SelectionMenu_StepCursors(struct SelectionMenu *p)
{
    struct SceneInputWidget *e = &p->cursors[0];
    s32 i = 2;
    do {
        Anim_Step(e, &p->panelResource);
        e++;
        i--;
    } while (i >= 0);
}

void SelectionMenu_RemovePanel(struct SelectionMenu *p)
{
    Anim_Remove(&p->panels[0]);
    Anim_Remove(&p->panels[1]);
    Sprite_Remove(&p->panel);
}

void SelectionMenu_RemoveCursors(struct SelectionMenu *p)
{
    struct SceneInputWidget *e = &p->cursors[0];
    s32 i = 2;
    do {
        Anim_Remove(e);
        e++;
        i--;
    } while (i >= 0);
}

void SelectionMenu_RemoveMarkerAnim(struct SelectionMenu *p)
{
    struct SceneInputWidget *e = p->markers;
    s32 i = 7;
    do {
        Anim_Remove(e);
        e++;
        i--;
    } while (i >= 0);
}

void SelectionMenu_RemoveHighlight(struct SelectionMenu *p)
{
    Anim_Remove(&p->highlight);
}

// confidence=low: lays out the 19 actor icons from the last one down, then the heading at x 32.
void sub_0801A6D0(struct SelectionMenu *p)
{
    struct ContactPosition position;
    struct SceneInputWidget *heading;
    int i;

    Position_Clear(&position);
    for (i = 18; i >= 0; i--)
        Widget_InitLinked(&p->icons[i], &p->iconResource, 0, 0x11, 0, 0, 0x3c, &position);
    heading = &p->heading;
    position.x = 0x20;
    position.y = 0;
    Widget_InitLinked(heading, &p->panelResource, 0, 0x11, 0, 0, 0x3c, &position);
}

void SelectionMenu_PopulateActors(struct SelectionMenu *p)
{
    struct SceneInputWidget *g = p->icons;
    int i;
    struct SelectionMenuPackedPosition v;
    for (i = 0; i < 16; i++, g++) {
        v.x = (i & 3) * 24 + 144;
        v.y = (i >> 2) * 24 + 32;
        SelectionMenu_SetActorIcon(p, g, i);
        *(struct SelectionMenuPackedPosition *)&g->position = v;
        if ((1 << i) & p->availableSlots) {
            g->paletteBank = 0x373;
            g->palette = SelectionMenu_GetIconPalette(0);
        } else {
            g->paletteBank = 0x374;
            g->palette = SelectionMenu_GetIconPalette(1);
        }
    }
    g = &p->heading;
    Anim_LoadKeyframe(g, &p->panelResource, 0x65, 0);
    g->flags &= ~1;
}

void SelectionMenu_PopulateItems(struct SelectionMenu *p)
{
    struct SceneInputWidget *g = p->icons;
    int i;
    struct SelectionMenuPackedPosition v;
    for (i = 0; i < 16; i++, g++) {
        v.x = (i & 3) * 24 + 144;
        v.y = (i >> 2) * 24 + 32;
        Presentation_SetItemGraphic(p, g, i);
        *(struct SelectionMenuPackedPosition *)&g->position = v;
        if ((1 << i) & p->availableSlots) {
            g->paletteBank = 0x373;
            g->palette = SelectionMenu_GetIconPalette(0);
        } else {
            g->paletteBank = 0x374;
            g->palette = SelectionMenu_GetIconPalette(1);
        }
    }
    g = &p->heading;
    Anim_LoadKeyframe(g, &p->panelResource, 0x66, 0);
    g->flags &= ~1;
}

void SelectionMenu_RemoveIconGrid(struct SelectionMenu *p)
{
    struct SceneInputWidget *e = p->icons;
    s32 i = 0x12;
    do {
        Anim_Remove(e);
        e++;
        i--;
    } while (i >= 0);
    Anim_Remove(&p->heading);
}

void SelectionMenu_StartAnimateChoice(struct SelectionMenu *p)
{
    if (p->choiceCount <= 1) {
        Sound_PlaySong(0xdd);
        SelectionMenu_SetCallback(p, SelectionMenu_AnimateChoice);
    }
}

void SelectionMenu_Back(struct SelectionMenu *p)
{
    struct SceneInputWidget *g = &p->cursors[1];
    if (p->choiceCount) {
        Sound_PlaySong(0xde);
        Anim_LoadKeyframe(g, &p->panelResource, 0x4d, 0);
        p->choiceCount--;
        (&p->icons[16 + p->choiceCount])->flags |= 1;
        if (!p->choiceCount)
            g->position.x = 16;
        else
            g->position.x = 64;
        g->position.y = 40;
        MessageWindow_SetBounds(2, 16, 30, 2);
        Presentation_UpdateActorGraphic(p, 0);
        SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    } else {
        Sound_PlaySong(0xde);
        SelectionMenu_SetCallback(p, SelectionMenu_Close);
    }
}

void SelectionMenu_AdvanceChoice(struct SelectionMenu *p)
{
    struct SceneInputWidget *g = &p->cursors[1];
    if (!p->choiceCount)
        g->position.x = 64;
    else
        g->position.x = 104;
    g->position.y = 40;
    if (++p->choiceCount <= 1)
        SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    else {
        sub_0801B600(p->choices[0], p->choices[1], p->result);
        SelectionMenu_SetCallback(p, SelectionMenu_ReviewChoice);
    }
}

// MATCHING_CONCESSION: register-pinning
void SelectionMenu_DrawStatistics(struct SelectionMenu *p)
{
    s16 k2 = 10;
    int value;
    int a;
    int b;
    SceneHud_DrawClock(p->statisticsPage);
    SceneHud_DrawNumericStatus(p->statisticsPage);
    value = gUnk_02000710->level;
    if (value > 99) {
        value = 99;
    } else if (value < 0) {
        value = 0;
    }
    {
        int x = 10;
        Text_DrawNumber(0, k2, 9, value, 33, 2, 15);
    }
    a = 0;
    b = 0;
    SceneInputPanel_GetEquippedValues(&a, &b, 0, 0);
    if (b > 0) {
        Text_DrawNumber(0, 9, 10, b, 33, 3, 15);
    } else {
        SceneHud_FillTile020(9, 10, 3);
    }
    if (a > 0) {
        Text_DrawNumber(0, 9, 11, a, 33, 3, 15);
    } else {
        SceneHud_FillTile020(9, 11, 3);
    }
    Text_DrawNumber(0, 11, 12, Player_GetCurrentAttribute(0), 33, 3, 15);
    Text_DrawNumber(0, 11, 13, Player_GetCurrentAttribute(1), 33, 3, 15);
    Text_DrawNumber(0, 11, 14, Player_GetCurrentAttribute(2), 33, 3, 15);
    SelectionMenu_StepCursors(p);
}

void SelectionMenu_ActorAvailability(struct SelectionMenu *p)
{
    int i, mask;
    s16 *slot;
    p->availableSlots = 0xffff;
    {
        struct GameState *base = gUnk_02000710;
        mask = 1;
        slot = base->inventory;
    }
    i = 7;
    do {
        if (*slot >= 0)
            p->availableSlots &= ~(mask << *slot);
        slot++;
        i--;
    } while (i >= 0);
    for (i = 0; i < 16; i++) {
        u8 *actor = Inventory_GetEquipmentEntry(i);
        if (*actor != 255) {
            int id = *actor;
            struct SelectionMenuActor *data = &gUnk_08E6092C[id];
            if (id != 46 && data->restrictions[0] == 0xffffffff &&
                (data->restrictions[1] & 0xffffff) == 0xffffff)
                p->availableSlots &= ~(1 << i);
        }
    }
}

void SelectionMenu_ItemAvailability(struct SelectionMenu *arg)
{
    struct SelectionMenu *p = arg;
    int i;
    int index;
    u8 alias;
    int mask;
    p->availableSlots = 0xffff;
    i = 0;
    mask = 1;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        index = Inventory_GetConsumableId(i);
        if (index >= 0) {
            struct SelectionMenuItem *table = gUnk_08E60F80;
            alias = table[index].available;
            if (alias && (!GameState_IsConsumableSlotEquipped(i))) {
                goto next;
            }
        }
        if (p) {
            p->availableSlots &= ~(mask << i);
        } else {
            p->availableSlots &= ~(mask << i);
        }
    next:
        i++;

    } while (i < 16);
}

void SelectionMenu_OpenActorPage(struct SelectionMenu *p)
{
    SelectionMenu_ActorAvailability(p);
    p->choiceCount = 0;
    p->statisticsPage = 0;
    Bg_ClearTilemap(0);
    SelectionMenu_ConfigureLayers(13, 0);
    SelectionMenu_PositionCursors(p);
    SelectionMenu_PositionInventoryMarkers(p);
    SelectionMenu_PopulateActors(p);
    MessageWindow_SetMessageTable(p->actorMessages);
    MessageWindow_SetBounds(2, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x81b);
}

void SelectionMenu_RefreshActorPage(struct SelectionMenu *p)
{
    SelectionMenu_ActorAvailability(p);
    SelectionMenu_PositionInventoryMarkers(p);
    SelectionMenu_PopulateActors(p);
    Presentation_UpdateActorGraphic(
        p, (u32)Text_LookupString(Script_ParseStringRef((void *)p->promptMessages)));
}

void SelectionMenu_RefreshItemPage(struct SelectionMenu *p)
{
    SelectionMenu_ItemAvailability(p);
    SelectionMenu_PositionEquipmentMarkers(p);
    SelectionMenu_PopulateItems(p);
    Presentation_UpdateItemGraphic(
        p, (u32)Text_LookupString(Script_ParseStringRef((void *)p->promptMessages)));
}

// MATCHING_CONCESSION: register-pinning
int SelectionMenu_NavigateGrid(struct SelectionMenu *p)
{
    int index = p->selectedSlot;
    int direction = 0;
    int pressedUp = gUnk_03005260[1] & 0x40;
    u16 *held = gUnk_03005260;
    if (pressedUp)
        direction = 1;
    else if (gUnk_03005260[1] & 0x80)
        direction = 2;
    else if (gUnk_03005260[1] & 0x20)
        direction = 4;
    else if (gUnk_03005260[1] & 0x10)
        direction = 8;
    if (!direction) {
        if (p->repeatTicks > 7) {
            if (*held & 0x40)
                direction = 1;
            else if (*held & 0x80)
                direction = 2;
            /* Read held input again for horizontal navigation. */
            asm("" ::: "memory");
            if (held[0] & 0x20)
                direction |= 4;
            else if (held[0] & 0x10)
                direction |= 8;
        } else if (gUnk_03005260[0] & 0xf0)
            p->repeatTicks++;
    }
    if (direction & 1) {
        if (!(index >> 2))
            index = (index & 3) + 12;
        else
            index -= 4;
        p->repeatTicks = 0;
    } else if (direction & 2) {
        int row = index >> 2;
        if (row == 3)
            index &= row;
        else
            index += 4;
        p->repeatTicks = 0;
    } else if (direction & 4) {
        if (!(index & 3))
            index = (index & 0xfc) + 3;
        else
            index--;
        p->repeatTicks = 0;
    } else if (direction & 8) {
        if ((index & 3) == 3)
            index &= 0xfc;
        else
            index++;
        p->repeatTicks = 0;
    }
    return index;
}

// MATCHING_CONCESSION: register-pinning
void SelectionMenu_Choose(struct SelectionMenu *p)
{
    int index;
    switch (p->choiceCount) {

        case 0:
            if (SelectionMenu_ConsumeCallbackFlag(p))
                SelectionMenu_RefreshActorPage(p);
            if (gUnk_03005260[0] & 4) {
                if (gUnk_03005260[1] & 1) {
                    if (++p->statisticsPage > 2)
                        p->statisticsPage = 0;
                    Sound_PlaySong(0xdc);
                }
            } else {
                index = SelectionMenu_NavigateGrid(p);
                if (index != p->selectedSlot) {
                    {
                        register int x asm("r0") = (index & 3) * 24 + 144;
                        /* Retain the existing pinned byte-offset calculation: typed
                         * coordinate stores coalesce the two address additions. */
                        register int offset asm("r3");
                        offset = 0x1b4;
                        asm("" : "+r"(offset));
                        *(u16 *)((u8 *)p + offset) = x;
                        x = (index >> 2) * 24 + 32;
                        offset += 2;
                        {
                            register u16 *out asm("r1") = (u16 *)((u8 *)p + offset);
                            *out = x;
                        }
                    }
                    p->selectedSlot = index;
                    Sound_PlaySong(0xdc);
                    Presentation_UpdateActorGraphic(p, 0);
                }
                if (gUnk_03005260[1] & 1) {
                    if (*Inventory_GetEquipmentEntry(p->selectedSlot) != 255 &&
                        ((1 << p->selectedSlot) & p->availableSlots))
                        SelectionMenu_StartAnimateChoice(p);
                    else
                        Sound_PlaySong(0x192);
                    return;
                } else if (gUnk_03005260[1] & 2) {
                    SelectionMenu_Back(p);
                    return;
                }
            }
            break;

        case 1:
            if (SelectionMenu_ConsumeCallbackFlag(p))
                SelectionMenu_RefreshItemPage(p);
            if (gUnk_03005260[0] & 4) {
                if (gUnk_03005260[1] & 1) {
                    if (++p->statisticsPage > 2)
                        p->statisticsPage = 0;
                    Sound_PlaySong(0xdc);
                }
            } else {
                index = SelectionMenu_NavigateGrid(p);
                {
                    register int old asm("r3") = p->selectedSlot;
                    asm("" : "+r"(old));
                    if (index != old) {
                        {
                            register int x asm("r0") = (index & 3) * 24 + 144;
                            /* Retain the existing pinned byte-offset calculation: typed
                         * coordinate stores coalesce the two address additions. */
                            register int offset asm("r3");
                            offset = 0x1b4;
                            asm("" : "+r"(offset));
                            *(u16 *)((u8 *)p + offset) = x;
                            x = (index >> 2) * 24 + 32;
                            offset += 2;
                            {
                                register u16 *out asm("r1") = (u16 *)((u8 *)p + offset);
                                *out = x;
                            }
                        }
                        p->selectedSlot = index;
                        Sound_PlaySong(0xdc);
                        Presentation_UpdateItemGraphic(p, 0);
                    }
                }
                if (gUnk_03005260[1] & 1) {
                    if (Inventory_GetConsumableId(p->selectedSlot) >= 0) {
                        int selected = p->selectedSlot;
                        if ((1 << selected) & p->availableSlots)
                            SelectionMenu_StartAnimateChoice(p);
                        else
                            Sound_PlaySong(0x192);
                    } else
                        Sound_PlaySong(0x192);
                    return;
                } else if (gUnk_03005260[1] & 2) {
                    SelectionMenu_Back(p);
                    return;
                }
            }
            break;
    }
    (p->frame)++;
}

void SelectionMenu_AnimateChoice(struct SelectionMenu *p)
{
    struct SceneInputWidget *source = &p->icons[p->selectedSlot];
    struct SceneInputWidget *dest = &p->icons[16 + p->choiceCount];
    if (SelectionMenu_ConsumeCallbackFlag(p)) {
        Widget_CopyMasked(dest, source, 13);
        dest->flags &= ~1;
        source->paletteBank = 0x374;
        source->palette = SelectionMenu_GetIconPalette(1);
        p->choices[p->choiceCount] = p->selectedSlot;
        p->availableSlots &= ~(1 << p->selectedSlot);
        if (!p->choiceCount)
            p->choiceTarget.x = 16;
        else
            p->choiceTarget.x = 64;
        p->choiceTarget.y = 40;
    }
    Vec16_Lerp(&dest->position, &source->position, &p->choiceTarget, p->frame, 3);
    if (p->frame > 7)
        SelectionMenu_AdvanceChoice(p);
    else
        (p->frame)++;
}

// MATCHING_CONCESSION: register-pinning
void SelectionMenu_ReviewChoice(struct SelectionMenu *p)
{
    struct SceneInputWidget *g;
    if (SelectionMenu_ConsumeCallbackFlag(p)) {
        g = &p->icons[18];
        Anim_LoadKeyframe(&p->cursors[0], &p->panelResource, 0x4d, 0);
        ScrollingDialogue_ClearScript();
        {
            u8 *result = p->result;
            Presentation_SetActorIconFrame(p, g, result);
        }
        g->flags &= ~1;
        g->position.x = 104;
        g->position.y = 40;
        p->horizontalChoice = 0;
        Anim_LoadKeyframe(&p->cursors[2], &p->panelResource, 0x4b, 0);
        p->cursors[2].flags &= ~1;
        MessageWindow_SetBounds(0, 16, 30, 2);
        MessageWindow_SetMessageTable(p->promptMessages);
        {
            struct SelectionMenuActor *actor = &gUnk_08E6092C[p->result[0]];
            if (gUnk_02000710->level >= actor->level || Inventory_HasItem(0x29))
                MessageWindow_ShowMessage(1);
            else
                MessageWindow_ShowMessage(2);
        }
        (&p->highlight)->flags |= 1;
        ScrollingDialogue_ClearScript();
    }
    if (gUnk_03005260[0] & 4) {
        if (gUnk_03005260[1] & 1) {
            if (++p->statisticsPage > 2)
                p->statisticsPage = 0;
            Sound_PlaySong(0xdc);
        }
    } else {
        SelectionMenu_HandleHorizontalInput(p);
        {
            int keys = gUnk_03005260[1], mask;
            mask = 1;
            asm("" : "+r"(mask));
            if (mask & keys) {
                if (!p->horizontalChoice) {
                    Sound_PlaySong(0xdd);
                    SelectionMenu_SetCallback(p, SelectionMenu_Confirm);
                    return;
                }
                goto back;
            } else if (2 & keys) {
            back:
                (&p->icons[18])->flags |= mask;
                p->cursors[2].flags |= mask;
                SelectionMenu_Back(p);
                return;
            }
            asm("" ::"r"(keys));
        }
    }
    (p->frame)++;
}

void SelectionMenu_Close(struct SelectionMenu *p)
{
    if (SelectionMenu_ConsumeCallbackFlag(p))
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
    if (p->frame == 20) {
        {
            u32 *state = &gUnk_0300523C;
            int mask;
            mask = ~4;
            *state &= mask;
        }
        if (p->cancelScript)
            Script_Call(p->cancelScript, 0);
    } else
        (p->frame)++;
}

void SelectionMenu_Hide(struct SelectionMenu *arg)
{
    struct SelectionMenu *p = arg;
    int i, mask;
    u32 *flags;
    u32 *state;
    struct SceneInputWidget *g;
    g = &p->panels[0];
    g->flags |= 1;
    g = &p->panels[1];
    g->flags |= 1;
    state = &gUnk_0300523C;
    mask = 1;
    flags = &p->icons[0].flags;
    i = 18;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    mask = 1;
    flags = &p->cursors[0].flags;
    i = 2;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    mask = 1;
    flags = &p->markers[0].flags;
    i = 7;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    g = &p->highlight;
    g->flags |= 1;
    {
        int clear = ~4;
        *state &= clear;
    }
}

void SelectionMenu_Confirm(struct SelectionMenu *p)
{
    if (SelectionMenu_ConsumeCallbackFlag(p))
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xffff, 0);
    if (p->frame > 31) {
        SelectionMenu_Hide(p);
        ScrollingDialogue_RequestHide();
        if (p->confirmScript)
            SelectionMenu_CallConfirmScript(p);
        Task_Kill(p);
    } else
        (p->frame)++;
}

u32 SelectionMenu_Update(struct SelectionMenu *p)
{
    Callback_InvokeActor(p, (void *)p->callback);
    SelectionMenu_DrawStatistics(p);
    return 0;
}

u32 SelectionMenu_Destroy(struct SelectionMenu *p)
{
    SelectionMenu_RemoveIconGrid(p);
    SelectionMenu_RemoveCursors(p);
    SelectionMenu_RemoveMarkerAnim(p);
    SelectionMenu_RemoveHighlight(p);
    SelectionMenu_RemovePanel(p);
    ScrollingDialogue_RequestHide();
    return 0;
}

// MATCHING_CONCESSION: register-pinning
int SelectionMenu_Initialize(struct SelectionMenu *p, void *a, void *b)
{
    struct SceneInputResource *resource = ScriptProc_Start(0xcb05, 0x530d);
    if (!resource)
        goto fail;
    asm("" ::: "r0");
    p->panelResource = *resource;
    ResourceHeader_Relocate(&p->panelResource, resource);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        goto fail;
    asm("" ::: "r0");
    p->iconResource = *resource;
    ResourceHeader_Relocate(&p->iconResource, resource);
    p->cancelScript = Script_GetKeywordValue('c', 0);
    p->confirmScript = Script_GetKeywordValue('e', 0);
    if (!Script_SeekToKeyword('w'))
        goto fail;
    if (!(p->actorMessages = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword('a'))
        goto fail;
    if ((p->itemMessages = Script_GetTextSourcePc()))
        goto configure;
fail:
    return -1;
configure:
    if (!Script_SeekToKeyword('k'))
        goto fail;
    if (!(p->promptMessages = Script_GetTextSourcePc()))
        goto fail;
    sub_0801A218(p);
    SelectionMenu_InitMarkers(p);
    sub_0801A6D0(p);
    SelectionMenu_InitCursors(p);
    SelectionMenu_InitHighlight(p);
    MessageWindow_SetBounds(0, 16, 30, 2);
    SelectionMenu_OpenActorPage(p);
    SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    ScrollingDialogue_RequestShow();
    {
        int mask = 4;
        gUnk_0300523C |= mask;
    }
    return 0;
}

void *SelectionMenu_Create(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0xDB4);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))SelectionMenu_Update,
                          (void (*)(void *))SelectionMenu_Destroy);
        if (SelectionMenu_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
