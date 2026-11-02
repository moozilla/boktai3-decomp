#include "monster_particle_random.h"
#include "global.h"
// Open: temporary choices only (constant 1 and rand address regs); pointer roles match.
#include "level_advance_display.h"
#include "engine.h"
#include "ram.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
struct InventoryPanelCallbackMetadata {
    u8 active, age;
    u8 unknown2[2];
    void *callback;
    u8 unknown8[0x38];
};
struct InventoryPanelCallback {
    struct LevelAdvanceParticle particle;
    void *callback;
};
struct InventoryPanelCallbackGroup {
    void *context;
    u8 active;
    u8 unknown5[3];
    struct InventoryPanelCallback entries[8];
};
struct InventoryPanel {
    u8 unknown0[0x4450];
    struct InventoryPanelCallbackGroup groups[2];
};
extern const s16 gUnk_086149C4[];
void *AnimData_FindEntry(s32);
u32 MenuParticle_UpdateMotion(void *);
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
    struct InventoryPanelCallbackGroup *group = &panel->groups[index];
    struct InventoryPanelCallbackMetadata *metadata;
    struct InventoryPanelCallback *entry;
    struct LevelAdvanceParticle *particle;
    s32 i;
    group->context = AnimData_FindEntry(0x1C1C);
    group->active = 1;
    i = 0;
    {
        const u16 *table = gRandomTable;
        metadata = (struct InventoryPanelCallbackMetadata *)(((u8 *)group) + 0x40);
        particle = &group->entries[0].particle;
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
