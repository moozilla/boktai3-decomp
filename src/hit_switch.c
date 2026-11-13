#include "hit_switch.h"
#include "player.h"

void HitSwitch_SetUpdate(struct HitSwitchEntry *entry, void (*update)(struct HitSwitchEntry *))
{
    entry->update = update;
    entry->frame = 0;
}

void TerrainModifiers_RegisterCell(struct SceneCellRef *, u32, u32, s32, u32, u32);

void HitSwitch_RegisterCompletedCell(struct HitSwitchEntry *entry)
{
    u32 kind;
    s32 height;
    if (entry->kind == 0) {
        kind = (entry->sprite.flags & 4) ? 2 : 1;
        {
            s32 y = entry->world.y;
            if (y >= 0)
                y = y >> 8;
            else
                y = -(-y >> 8);
            height = y + 1;
        }
    } else {
        kind = 0;
        {
            s32 y = entry->world.y;
            if (y >= 0)
                height = y >> 8;
            else
                height = -(-y >> 8);
        }
    }
    TerrainModifiers_RegisterCell(&entry->cellRef, entry->cellIndex, kind, height, 255, 0);
    entry->cellRegistered = 1;
}

void HitSwitch_ReceiveCollision(struct SceneCollider *other, void *unused,
                                struct HitSwitchEntry *entry)
{
    u8 *cooldown = &entry->hitCooldown;
    if (*cooldown == 0) {
        u32 mask = 0x20;
        if (other->contactFlags2 & mask) {
            entry->hitCount++;
            *cooldown = mask;
            gHitSwitchManager->hitPending = 1;
        }
    }
}

void HitSwitch_UpdateStatic(struct HitSwitchEntry *entry)
{
    if (entry->hitCount >= entry->requiredHits) {
        u16 *state;
        u32 flags;
        u32 mask;
        entry->completed = 1;
        HitSwitch_RegisterCompletedCell(entry);
        state = &entry->collider.state;
        flags = *state;
        mask = 4;
        mask |= flags;
        *state = mask;
        HitSwitch_SetUpdate(entry, HitSwitch_AnimateStaticCompletion);
    }
}

void HitSwitch_AnimateStaticCompletion(struct HitSwitchEntry *entry)
{
    entry->sprite.priority = 2;
    if (entry->frame == 0)
        Sound_PlaySong(426);
    entry->frame++;
    if (entry->frame <= 7)
        entry->sprite.frame = 1;
    else {
        entry->sprite.frame = 2;
        HitSwitch_SetUpdate(entry, HitSwitch_TickCompletedStatic);
        if (entry->completedScriptId) {
            s32 value;
            struct HitSwitchScriptArgs args;
            args.count = 1;
            args.values = &value;
            value = entry->tag;
            Script_Call(entry->completedScriptId, &args);
        }
    }
}

void HitSwitch_TickCompletedStatic(struct HitSwitchEntry *entry)
{
}

void HitSwitch_UpdateBobbing(struct HitSwitchEntry *entry)
{
    if (entry->bobPeriod) {
        s32 phase = Div(entry->frame * 255, entry->bobPeriod);
        s32 raw = gUnk_086149C4[phase & 255] * 12;
        s32 offset;
        if (raw >= 0)
            offset = raw >> 12;
        else
            offset = -(-raw >> 12);
        entry->sprite.position.y = (u16)entry->world.y + offset;
        entry->frame++;
        if (entry->frame >= entry->bobPeriod)
            entry->frame = 0;
    }
    if (entry->hitCount >= entry->requiredHits) {
        u16 *state;
        u32 flags;
        entry->completed = 1;
        HitSwitch_RegisterCompletedCell(entry);
        state = &entry->collider.state;
        flags = *state;
        {
            u32 mask = 4;
            mask |= flags;
            *state = mask;
        }
        HitSwitch_SetUpdate(entry, HitSwitch_AnimateBobbingCompletion);
    }
}

void HitSwitch_AnimateBobbingCompletion(struct HitSwitchEntry *entry)
{
    s32 k2 = 0;
    u16 *frame = &entry->frame;
    if ((*frame) == k2) {
        Sound_PlaySong(426);
    }
    {
        u32 next = *frame + 1;
        *frame = next;
        if ((u16)next <= 7) {
            entry->sprite.position = *(struct ContactPosition *)&entry->world;
            entry->sprite.frame = 1;
        } else {
            entry->sprite.frame = 2;
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateCompletedBobbing);
            if (entry->completedScriptId) {
                s32 value;
                struct HitSwitchScriptArgs args;
                args.count = 1;
                args.values = &value;
                value = entry->tag;
                Script_Call(entry->completedScriptId, &args);
            }
        }
    }
}

void HitSwitch_UpdateCompletedBobbing(struct HitSwitchEntry *entry)
{
    if (gPlayers[0]->recoveryTerrainCell == entry->cellIndex) {
        if (entry->sprite.position.y == entry->world.y)
            Sound_PlaySong(691);
        if (entry->sprite.position.y > entry->world.y - 16)
            entry->sprite.position.y -= 2;
    } else if (entry->sprite.position.y < entry->world.y)
        entry->sprite.position.y += 2;
}

static inline u32 HitSwitch_MaskChannel(u32 value, const u32 *mask)
{
    return value & *mask;
}

void HitSwitch_UpdateSharedFlash(struct HitSwitchManager *manager)
{
    if (manager->hitPending) {
        Sound_PlaySong(425);
        manager->flash = 32;
        manager->hitPending = 0;
    }
    if (manager->flash) {
        manager->flash--;
        if (manager->flash == 0)
            CpuSet(gPaletteBankData + 0x1AA0, manager->palette, 0x04000008);
        else if (manager->flash > 15)
            CpuSet(gPaletteBankData + 0x1AC0, manager->palette, 0x04000008);
        else {
            s32 flash = manager->flash;
            s32 inverse = 16 - flash;
            u16 *from = (u16 *)(gPaletteBankData + 0x1AC0);
            u16 *to = (u16 *)(gPaletteBankData + 0x1AA0);
            s32 colorIndex;
            u32 channelMask;
            colorIndex = 0;
            channelMask = 31;
            do {
                u32 fromColor = *from;
                s32 fromRed = 31;
                s32 fromGreen;
                u32 toColor;
                s32 toRed;
                s32 toGreen;
                s32 red, green, blue;
                fromRed &= fromColor;
                fromColor <<= 16;
                fromGreen = fromColor >> 21;
                fromGreen = HitSwitch_MaskChannel(fromGreen, &channelMask);
                fromColor >>= 26;
                fromColor = HitSwitch_MaskChannel(fromColor, &channelMask);
                toColor = *to;
                toRed = 31;
                toRed &= toColor;
                toColor <<= 16;
                toGreen = toColor >> 21;
                toGreen = HitSwitch_MaskChannel(toGreen, &channelMask);
                toColor >>= 26;
                toColor = HitSwitch_MaskChannel(toColor, &channelMask);
                red = (fromRed * flash + toRed * inverse) >> 4;
                green = (fromGreen * flash + toGreen * inverse) >> 4;
                blue = ((s32)fromColor * flash + (s32)toColor * inverse) >> 4;
                manager->palette[colorIndex] = (blue << 10) | (green << 5) | red;
                from++;
                to++;
            } while (++colorIndex < 16);
        }
    }
}

s32 HitSwitch_UpdateManager(struct HitSwitchManager *manager)
{
    s32 i;
    HitSwitch_UpdateSharedFlash(manager);
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        entry->update(entry);
        if (entry->hitCooldown) {
            entry->hitCooldown--;
            if (entry->hitCooldown) {
                entry->animation.paletteIndex = 214;
                entry->animation.palette = manager->palette;
            } else
                AnimData_SelectPaletteBank(&entry->animation, 213);
        }
    }
    return 0;
}

void SceneBody_Unregister(struct SceneBody *);

void TerrainModifiers_UnregisterCell(struct SceneCellRef *);

s32 HitSwitch_DestroyManager(struct HitSwitchManager *manager)
{
    s32 i;
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        if (entry->bodyEnabled)
            SceneBody_Unregister(&entry->body);
        Sprite_Remove(&entry->sprite);
        Collider_Remove(&entry->collider);
        if (entry->cellRegistered)
            TerrainModifiers_UnregisterCell(&entry->cellRef);
    }
    gHitSwitchManager = 0;
    return 0;
}

void HitSwitch_ReceiveCollision(struct SceneCollider *, void *, struct HitSwitchEntry *);

void HitSwitch_InitCollider(struct HitSwitchEntry *entry)
{
    struct HitSwitchExtent size, offset;
    struct SceneCollider *collider = &entry->collider;
    size.a.lo = 0x80;
    size.a.hi = 0x78;
    size.b.lo = 0x80;
    *(u32 *)&offset.a = 0x7f0000;
    *(u32 *)&offset.b &= 0xffff0000;
    Collider_Init(collider, entry->collisionId, 0x4000, 0, 0x10, &size, &offset);
    Collider_SetCallback(collider, HitSwitch_ReceiveCollision, entry);
    Collider_SetPosition(collider, &entry->world, 0);
    Collider_Refresh(collider);
}

void SceneBody_Init(struct SceneBody *, u32, struct SceneVec *, u32, u32, struct HitSwitchEntry *);

void TerrainOverlay_QueueAddActor(struct SceneBody *);

void HitSwitch_InitEntryFromScript(struct HitSwitchManager *input)
{
    struct HitSwitchManager *manager = input;
    struct HitSwitchEntry *entry;
    struct SceneVec *world;
    struct SceneAnimation *animation;
    struct DisplaySequenceSprite *sprite;
    u32 present;
    entry = &manager->entries[manager->count];
    {
        u32 found = Script_SeekToKeyword('n');
        if (found)
            entry->collisionId = Script_GetValue();
        else
            entry->collisionId = found;
    }
    present = Script_SeekToKeyword('p');
    if (present) {
        entry->world.x = Script_GetValue();
        entry->world.y = Script_GetValue();
        entry->world.z = Script_GetValue();
        world = &entry->world;
    } else {
        entry->world.x = present;
        entry->world.y = present;
        entry->world.z = present;
        world = &entry->world;
    }
    {
        s32 x = (s16)(u16)world->x >> 8;
        s32 z = (s16)(u16)world->z >> 8;
        u32 index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        entry->cellIndex = index;
    }
    if (Script_SeekToKeyword('t'))
        entry->kind = Script_GetValue();
    else
        entry->kind = 0;
    {
        u32 kind = entry->kind;
        u32 resource;
        resource = 0xD166;
        if (kind == 0)
            resource = 0xC3C3;
        sprite = &entry->sprite;
        animation = &entry->animation;
        AnimData_Load(animation, resource);
        Sprite_Init(sprite, animation, 0);
        sprite->priority = 3;
    }
    if (Script_SeekToKeyword('r') && Script_GetValue())
        sprite->flags |= 4;
    sprite->position = *(struct ContactPosition *)world;
    if (Script_SeekToKeyword('h')) {
        entry->bodyEnabled = Script_GetValue();
        if (entry->bodyEnabled) {
            struct SceneBody *body = &entry->body;
            SceneBody_Init(body, entry->collisionId, (struct SceneVec *)&sprite->position, 0, 7,
                           entry);
            TerrainOverlay_QueueAddActor(body);
        }
    } else
        entry->bodyEnabled = 0;
    entry->hitCount = 0;
    if (Script_SeekToKeyword('A'))
        entry->requiredHits = Script_GetValue();
    else
        entry->requiredHits = 10;
    {
        u32 period = Script_SeekToKeyword('W');
        if (period)
            period = Script_GetValue();
        {
            u16 *bobPeriod = &entry->bobPeriod;
            *bobPeriod = period;
        }
    }
    HitSwitch_InitCollider(entry);
    {
        u8 *cooldown = &entry->hitCooldown;
        u32 zero = 0;
        *cooldown = zero;
    }
    {
        u32 found = Script_SeekToKeyword('s');
        if (found)
            entry->completed = Script_GetValue();
        else
            entry->completed = found;
    }
    if (entry->completed) {
        u16 *state;
        u32 flags, mask;
        HitSwitch_RegisterCompletedCell(entry);
        state = &entry->collider.state;
        flags = *state;
        mask = 4;
        mask |= flags;
        *state = mask;
        sprite->frame = 2;
        if (entry->kind == 0) {
            sprite->priority = 2;
            HitSwitch_SetUpdate(entry, HitSwitch_TickCompletedStatic);
        } else
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateCompletedBobbing);
    } else {
        entry->cellRegistered = 0;
        sprite->frame = 0;
        sprite->priority = 3;
        if (entry->kind == 0)
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateStatic);
        else
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateBobbing);
    }
    if (Script_SeekToKeyword('g')) {
        entry->tag = Script_GetValue();
        entry->completedScriptId = Script_GetValue();
    } else {
        entry->tag = -1;
        entry->completedScriptId = 0;
    }
    {
        u32 count = manager->count + 1;
        manager->count = count;
    }
}

struct HitSwitchManager *HitSwitch_CreateManager(void)
{
    struct HitSwitchManager *manager = Task_Create(8, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))HitSwitch_UpdateManager,
                          (void (*)(void *))HitSwitch_DestroyManager);
        manager->count = 0;
    }
    gHitSwitchManager = manager;
    return manager;
}

u32 HitSwitch_GetCompletionFromScript(void)
{
    struct HitSwitchManager *manager = gHitSwitchManager;
    if (manager) {
        s32 tag;
        s32 i;
        if (Script_SeekToKeyword('i'))
            tag = Script_GetValue();
        else
            tag = 0;
        for (i = 0; i < manager->count; i++)
            if (manager->entries[i].tag == tag)
                return manager->entries[i].completed;
    }
    return 0;
}

struct HitSwitchManager;

extern struct HitSwitchManager *gHitSwitchManager;

struct HitSwitchManager *HitSwitch_CreateManager(void);

/* Declared with a result here: the original forwards whatever r0 holds when
   this (really void) routine returns. */

s32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gHitSwitchManager;
    if (manager == 0)
        manager = HitSwitch_CreateManager();
    return ((s32(*)(struct HitSwitchManager *))HitSwitch_InitEntryFromScript)(manager);
}
