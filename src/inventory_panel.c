#include "global.h"
#include "scene_item_values.h"
#include "game_state.h"
#include "player.h"
#include "inventory_panel.h"
#include "scene_hud_display.h"
#include "level_advance_display.h"
#include "monster_particle_random.h"
#include "engine.h"
#include "ram.h"

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 GameState_GetUnlockEntry(s32);

void ActionRecord_CopyFromEntry(struct SceneValueEquipment *, void *);

void *Player_CalculatePreviewActionStat(void *, s32, void *, struct SceneValueEquipment *, s32);

s32 InventoryPanel_CreateEntry(void **out, s32 position, s32 index, s32 slotMode, s32 kind)
{
    s32 result;
    u8 data[0x20];
    struct SceneValueEquipment *equipment = 0;
    if (slotMode) {
        if (index > 7) {
            kind = 4;
            data[0] = GameState_GetUnlockEntry(
                          ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]) -
                      1;
        } else {
            kind = 0;
            equipment = Inventory_GetEquipmentEntry(
                ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]);
            ActionRecord_CopyFromEntry(equipment, data);
        }
    } else if (kind == 4) {
        data[0] = GameState_GetUnlockEntry(index) - 1;
    } else {
        kind = 0;
        equipment = Inventory_GetEquipmentEntry(index);
        ActionRecord_CopyFromEntry(equipment, data);
    }
    if (data[0] != 255) {
        *out = Player_CalculatePreviewActionStat((*(void **)gPlayers), position, data, equipment,
                                                 kind);
        result = kind;
    } else {
        result = -1;
    }
    return result;
}

struct MenuDigit {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y;
    u8 unknown24[0x3C];
};

struct MenuDisplay {
    u8 unknown0[0x1420];
    struct MenuDigit digits[1];
};

void Hud_FillDigitCells(s32, s32, s32, s32);

void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *, s32, s32, s32, s32);

void InventoryPanel_SetEntryRow(struct MenuDisplay *p, s32 x, s32 y, s32 value, s32 frameBase,
                                s32 first)
{
    s32 i;
    if (value < 0) {
        for (i = 0; i < 3; i++)
            p->digits[first + i].flags |= 1;
        Hud_FillDigitCells(3, x, y, 0);
    } else {
        Hud_FillDigitCells(3, x, y, -1);
        for (i = 0; i < 3; i++) {
            p->digits[first + i].x = x * 8 + i * 8;
            p->digits[first + i].y = y * 8;
            p->digits[first + i].flags &= ~1;
        }
        MenuDisplay_LoadDecimalDigits(p, value, 100, frameBase, first);
    }
}

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 InventoryPanel_CreateEntry(void **, s32, s32, s32, s32);

void InventoryPanel_SetEntryRow(struct MenuDisplay *, s32, s32, s32, s32, s32);

static inline void HideSelected(struct InventoryPanel *p, s32 x, s32 y)
{
    u32 mask = 1;
    struct InventoryPanelElement *e = p->selected;
    s32 i = 2;
    do {
        e->flags |= mask;
        e++;
    } while (--i >= 0);
    Hud_FillDigitCells(3, x, y, 0);
}

s32 InventoryPanel_UpdateSelectedUnlock(struct InventoryPanel *p, s32 x, s32 y, s32 query,
                                        s32 position)
{
    void *value;
    s32 kind = InventoryPanel_CreateEntry(&value, position, (s16)gGameState->unk50, 1, 0);
    if (query) {
        if (kind != 4) {
            kind = -1;
            goto done;
        }
    } else {
        if (kind != 4) {
            HideSelected(p, x, y);
            return -1;
        }
        if ((s32)value > 0)
            InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, (s32)value, query, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = (s32)value;
done:
    return kind;
}

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 InventoryPanel_UpdateSelectedEquipment(struct InventoryPanel *p, s32 x, s32 y, s32 query,
                                           s32 position)
{
    void *value;
    s32 kind = InventoryPanel_CreateEntry(&value, position, (s16)gGameState->unk50, 1, 0);
    if (query) {
        if (kind != 0) {
            kind = -1;
            goto done;
        }
    } else {
        if (kind != 0) {
            Hud_FillDigitCells(3, x, y, 0);
            return -1;
        }
        if ((s32)value > 0)
            InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, (s32)value, kind, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = (s32)value;
done:
    return kind;
}

s32 InventoryPanel_UpdateSelectedUnlock(struct InventoryPanel *, s32, s32, s32, s32);

s32 InventoryPanel_UpdateSelectedEquipment(struct InventoryPanel *, s32, s32, s32, s32);

static inline void DrawComparison(void *p, s32 x, s32 y, s32 value, s32 delta, s32 old)
{
    if (delta > 0)
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, value, 10, 0x47);
    else if (delta < 0)
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, value, 20, 0x47);
    else
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, old, 0, 0x47);
}

void InventoryPanel_DrawStatComparison(u8 *p, s32 unlock, s32 equipment, s32 selected, s32 active,
                                       s32 position)
{
    s32 value, old, delta, item, kind;
    if (active) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (selected) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (equipment) {
        if (InventoryPanel_CreateEntry(&value, position, p[0x4387], 0, 0) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        }
    } else if (unlock) {
        if (InventoryPanel_CreateEntry(&value, 0, p[0x4387] - 8, 0, 4) < 0)
            InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, 0);
        else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            DrawComparison(p, 3, 14, value, delta, old);
        }
    }
}

#define STATUS_WIDGET 79

static inline u32 Offset(u32 base, u32 off)
{
    u32 sum = base + off;
    return sum;
}

static inline void PlaceStatusIcon(struct SceneHudState *p, s32 n,
                                   struct SceneInputPackedPosition pos)
{
    u32 off = (n + STATUS_WIDGET) * sizeof(struct SceneInputWidget);
    *(u32 *)Offset((u32)&p->categoryWidgets[0].flags, off) &= ~1;
    *(struct SceneInputPackedPosition *)Offset((u32)&p->categoryWidgets[0].position, off) = pos;
}

void InventoryPanel_ShowStatusIcons(struct SceneHudState *p)
{
    struct SceneInputPackedPosition pos;
    s32 n = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (((struct GameState *)(p->actionOwner->timedEventSlot * 4 + (u32)gGameState))
            ->counters628[0] > 0) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5e, 1);
        p->statusIcons[n].flags &= ~1;
        *(struct SceneInputPackedPosition *)&p->statusIcons[n].position = pos;
        pos.x = 0xd;
        n = 1;
    }
    if (p->actionOwner->movementModifierActive) {
        if (p->actionOwner->movementModifierKind == 1)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5c, 1);
        else if (p->actionOwner->movementModifierKind == 2)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5b, 1);
        else
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5d, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    } else if (p->actionOwner->scriptStatus[0]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5f, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    }
    if (p->actionOwner->scriptStatus[1]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x60, 1);
        PlaceStatusIcon(p, n, pos);
        n++;
    }
    for (; n <= 3; n++)
        p->statusIcons[n].flags |= 1;
}

struct MenuParticle {
    u32 flags;
    u8 unknown4[0x14];
    u16 x, y;
    u8 unknown1C[0x14];
    u16 velocityX, velocityY;
    u8 unknown34[4];
    u8 active, age;
};

u32 MenuParticle_UpdateMotion(struct MenuParticle *particle)
{
    if (++particle->age > 19) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        particle->x += particle->velocityX;
        particle->y += particle->velocityY;
        if (!(particle->age & 3))
            particle->velocityY++;
    }
    return particle->active;
}

/* Tentative inventory display family, inferred from slot and item consumers. */

void InventoryPanel_UpdateCallbackGroups(struct InventoryPanel *panel)
{
    s32 i = 0;
    do {
        s32 offset = ((i << 6) + i) << 3;
        struct InventoryPanelCallbackGroup *group;
        s32 enabled;
        s32 next;
        offset += 0x4450;
        group = (struct InventoryPanelCallbackGroup *)((u32)panel + offset);
        enabled = group->active;
        next = i + 1;
        if (enabled) {
            s32 active = 0;
            struct InventoryPanelCallbackMetadata *metadata =
                group->callbacks.metadataView.metadata;
            struct InventoryPanelCallback *entry = group->callbacks.entries;
            s32 j = 7;
            do {
                if (metadata->active &&
                    sub_08249240(&entry->particle, group->context, metadata->callback))
                    active = 1;
                metadata++;
                entry++;
            } while (--j >= 0);
            group->active = active;
        }
        i = next;
    } while (i <= 1);
}

// Open: temporary choices only (constant 1 and rand address regs); pointer roles match.

/* Tentative inventory display family, inferred from slot and item consumers. */

extern const s16 gUnk_086149C4[];

void *AnimData_FindEntry(s32);

u32 MenuParticle_UpdateMotion(struct MenuParticle *);

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline u32 Arg1(u32 k_, s32 angle)
{
    return angle + k_;
}

void InventoryPanel_StartCallbackBurst(struct InventoryPanel *panel, s32 index, s32 x, s32 y)
{
    s32 offset = ((index << 6) + index) << 3;
    struct InventoryPanelCallbackGroup *group =
        (struct InventoryPanelCallbackGroup *)((u8 *)panel + 0x4450 + offset);
    struct InventoryPanelCallbackMetadata *metadata;
    struct InventoryPanelCallback *entry;
    struct LevelAdvanceParticle *particle;
    s32 i;
    group->context = AnimData_FindEntry(0x1C1C);
    group->active = 1;
    i = 0;
    {
        const u16 *table = gRandomTable;
        metadata = group->callbacks.metadataView.metadata;
        particle = &group->callbacks.entries[0].particle;
        entry = (struct InventoryPanelCallback *)particle;
        for (; i < 8; i++) {
            s32 angle;
            s32 scale;
            u32 flags = entry->particle.flags & (~1);
            entry->particle.flags = flags;
            if ((u8)ParticleRandom_ReadSample(table) & 1)
                flags |= 4;
            else
                flags &= ~4;
            entry->particle.flags = flags;
            SpriteDrawable_SelectTileFrame(particle, group->context, 0x15);
            Effect_SetPalette(particle, 0);
            angle = ((i * 32) + (((s32)ParticleRandom_ReadSample(table) >> 3) & 0xf)) - 8;
            scale = (((s32)ParticleRandom_ReadDirectSample(table) >> 3) & 1) + 3;
            entry->particle.velocityX = Scale(gUnk_086149C4[Arg1(0x40, angle) & 0xff] * scale);
            entry->particle.velocityY = Scale(gUnk_086149C4[angle & 0xff] * scale) - 2;
            entry->particle.position.world.x = entry->particle.velocityX + x;
            entry->particle.position.world.y = entry->particle.velocityY + y;
            metadata->active = 1;
            metadata->age = 0;
            entry->callback = MenuParticle_UpdateMotion;
            metadata++;
            entry++;
            particle = (struct LevelAdvanceParticle *)(((u8 *)particle) + 0x40);
        }
    }
}

struct PanelParticle {
    u8 unknown[0x40];
};

struct PanelParticleGroup {
    struct PanelParticle particles[8];
    u8 unknown200[8];
};

struct PanelEffect {
    u8 unknown[0x28];
};

struct PlayerPanel {
    u8 unknown0[0x4458];
    struct PanelParticleGroup groups[2];
    u8 unknown4868[0x478];
    struct PanelEffect effects[4];
};

void PlayerPanel_RemoveEffects(struct PlayerPanel *panel)
{
    s32 group = 0;
    u32 offset = 0;
    s32 i;
    do {
        struct PanelParticle *particle = (struct PanelParticle *)((u8 *)panel + (offset + 0x4458));
        for (i = 7; i >= 0; i--) {
            Effect_Remove(particle);
            particle++;
        }
        offset += 0x208;
        (*(&group))++;
    } while (group < 2);
    {
        struct PanelEffect *effect = panel->effects;
        for (group = 3; group >= 0; group--) {
            Effect_Remove(effect);
            effect++;
        }
    }
}

/* Tentative inventory display family, inferred from slot and item consumers. */

void InventoryPanel_InitCallbackGroups(struct InventoryPanel *panel)
{
    s32 i = 0;
    do {
        s32 offset = ((i << 6) + i) << 3;
        struct InventoryPanelCallbackGroup *group;
        s32 frame;
        s32 negative;
        u32 zero;
        s32 next;
        struct InventoryPanelCallbackMetadata *metadata;
        struct InventoryPanelCallback *entry;
        s32 j;
        offset += 0x4450;
        group = (struct InventoryPanelCallbackGroup *)((u32)panel + offset);
        frame = 0x91;
        if (i == 0)
            frame = 0x11;
        group->context = AnimData_FindEntry(0x1C1E);
        group->active = 0;
        group->initialized = 0;
        next = i + 1;
        negative = -4;
        zero = 0;
        metadata = group->callbacks.metadataView.metadata;
        entry = group->callbacks.entries;
        j = 7;
        do {
            ParticleSprite_Initialize(&entry->particle, group->context, frame);
            Effect_SetOrigin(&entry->particle, negative, negative);
            entry->particle.mode = zero;
            metadata->active = zero;
            metadata++;
            entry++;
        } while (--j >= 0);
        i = next;
    } while (i <= 1);
}
