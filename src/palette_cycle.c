#include "palette_cycle.h"

void PaletteCycle_RefreshBasePalette(struct PaletteCycle *cycle)
{
    CpuSet(gBgPaletteBuffer, cycle->colors, 0x04000080);
}

u32 PaletteCycle_UpdateEntry(struct PaletteCycle *unused, struct PaletteCycleEntry *entry)
{
    u16 *from, *to;
    s32 length, amount, inverse, i;
    if (entry->reverse == 0) {
        if (entry->step >= entry->length[0]) {
            if (entry->held >= entry->hold[0]) {
                if (!(entry->flags & 2)) {
                    entry->reverse = 1;
                    entry->step = 0;
                    entry->held = 0;
                    from = entry->target;
                    to = entry->source;
                    length = entry->length[1];
                } else {
                    from = entry->source;
                    to = entry->target;
                    length = entry->length[0];
                }
            } else {
                from = entry->source;
                to = entry->target;
                length = entry->length[0];
                entry->held++;
            }
        } else {
            from = entry->source;
            to = entry->target;
            length = entry->length[0];
            entry->step++;
        }
    } else {
        if (entry->step >= entry->length[1]) {
            if (entry->held >= entry->hold[1]) {
                if (!(entry->flags & 4)) {
                    entry->reverse = 0;
                    entry->step = 0;
                    entry->held = 0;
                    from = entry->source;
                    to = entry->target;
                    length = entry->length[0];
                } else {
                    from = entry->target;
                    to = entry->source;
                    length = entry->length[1];
                }
            } else {
                from = entry->target;
                to = entry->source;
                length = entry->length[1];
                entry->held++;
            }
        } else {
            from = entry->target;
            to = entry->source;
            length = entry->length[1];
            entry->step++;
        }
    }
    if (entry->flags & 1) {
        if (length > 0)
            amount = Div(entry->step << 5, length);
        else
            amount = 0;
        inverse = 32 - amount;
        for (i = 0; i < entry->count; i++) {
            u32 x = from[i], y = to[i];
            u32 rb, g, c;
            rb = ((x & 0x7C1F) * amount + (y & 0x7C1F) * inverse) & 0xF83E0;
            g = ((x & 0x3E0) * amount + (y & 0x3E0) * inverse) & 0x7C00;
            c = (rb | g) >> 5;
            entry->destination[i] = c;
        }
    } else {
        for (i = 0; i < entry->count; i++)
            entry->destination[i] = to[i];
    }
    return 0;
}

u32 PaletteCycle_Update(struct PaletteCycle *cycle)
{
    u32 i;
    struct PaletteCycleEntry *entry;
    if (Palette_IsDirty() != 0)
        PaletteCycle_RefreshBasePalette(cycle);
    i = 0;
    if (i < cycle->entryCount) {
        entry = cycle->entries;
        do {
            PaletteCycle_UpdateEntry(cycle, entry);
            entry++;
            i++;
        } while (i < cycle->entryCount);
    }
    return 0;
}

void PaletteCycle_Destroy(void)
{
    gPaletteCycle = 0;
}

s32 PaletteCycle_Initialize(struct PaletteCycle *cycle)
{
    gPaletteCycle = (u32)cycle;
    cycle->entryCount = 0;
    PaletteCycle_RefreshBasePalette(cycle);
    return 0;
}

struct PaletteCycle *PaletteCycle_Create(void)
{
    struct PaletteCycle *cycle;
    if (gPaletteCycle != 0)
        return (void *)gPaletteCycle;
    cycle = Task_Create(0xC, sizeof(*cycle));
    if (cycle != 0) {
        Task_SetCallbacks(cycle, (void *)PaletteCycle_Update, (void *)PaletteCycle_Destroy);
        if (PaletteCycle_Initialize(cycle) < 0) {
            Task_Kill(cycle);
            return 0;
        }
    }
    return cycle;
}

/* Registry routine 3A89: adds a palette blend entry to the palette blend task
   (0x02000094, created on demand; up to 16): -s and -d palette, color and
   bank (1 = the task's own colors, else the palette buffer), -b palette and
   color in the palette buffer, -n (1), -f, -S (10), -D (10), -w, -W.
   Returns -1 when full (tentative). */

s32 PaletteBlend_AddFromScript(void)
{
    struct PaletteCycle *cycle = PaletteCycle_Create();
    struct PaletteCycleEntry *entry;
    s32 sourceBank, sourceColor, sourceSaved, targetBank, targetColor, targetSaved, outputBank,
        outputColor;
    if (!cycle)
        return -1;
    if (cycle->entryCount > 15)
        return -1;
    entry = &cycle->entries[cycle->entryCount];
    if (Script_SeekToKeyword('s')) {
        sourceBank = Script_GetValue();
        sourceColor = Script_GetValue();
        sourceSaved = Script_GetValue();
    } else {
        sourceBank = 0;
        sourceColor = 0;
        sourceSaved = 0;
    }
    if (Script_SeekToKeyword('d')) {
        targetBank = Script_GetValue();
        targetColor = Script_GetValue();
        targetSaved = Script_GetValue();
    } else {
        targetBank = 0;
        targetColor = 0;
        targetSaved = 0;
    }
    if (Script_SeekToKeyword('b')) {
        outputBank = Script_GetValue();
        outputColor = Script_GetValue();
    } else {
        outputBank = 0;
        outputColor = 0;
    }
    if (sourceSaved == 1)
        entry->source = &cycle->colors[sourceBank * 16 + sourceColor];
    else
        entry->source = &((u16(*)[16])gBgPaletteBuffer)[sourceBank][sourceColor];
    if (targetSaved == 1)
        entry->target = &cycle->colors[targetBank * 16 + targetColor];
    else
        entry->target = &((u16(*)[16])gBgPaletteBuffer)[targetBank][targetColor];
    entry->destination = &((u16(*)[16])gBgPaletteBuffer)[outputBank][outputColor];
    entry->count = Script_GetKeywordValue('n', 1);
    entry->step = 0;
    entry->held = 0;
    entry->flags = Script_GetKeywordValue('f', 0);
    entry->reverse = 0;
    entry->length[0] = Script_GetKeywordValue('S', 10);
    entry->length[1] = Script_GetKeywordValue('D', 10);
    entry->hold[0] = Script_GetKeywordValue('w', 0);
    entry->hold[1] = Script_GetKeywordValue('W', 0);
    cycle->entryCount++;
}
