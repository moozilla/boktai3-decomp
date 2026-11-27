#include "global.h"

struct DisplayAnimation {
    u8 unk_00[0x94];
};
struct PaletteDisplayEntry {
    u16 id;
    u16 frames;
    u8 unk_04[8];
    u16 palette;
    u8 directionIndex;
    u8 flags;
};
struct PaletteTransition {
    u8 unk_00[0x18];
    s32 timer;
    u8 unk_1C[0xc];
    struct DisplayAnimation animations[5];
    u8 unk_30C[0x9ec - 0x30c];
    s16 x, y, z;
    u8 unk_9F2[0xc];
    u8 dirX, dirY;
    u16 direction;
    u8 unk_A02[6];
    const struct PaletteDisplayEntry *entry;
};
struct Position {
    s16 x, y, z;
};
extern const s8 gUnk_086126CC[];
extern const u16 gUnk_086126D6[];
s32 DisplayAnimation_InitializeSequence(void *, u32, u32, u32, void *);
void DisplayAnimation_ClearFlags(void *, u32);
void DisplayAnimation_SelectPaletteBank(void *, u32);
void DisplayAnimation_SetPriority(void *, u32);
void Dir_Decompose(u8, u8 *, u8 *, u32);
void DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);
void DisplayAnimation_UpdatePlayback(void *);

void PaletteTransition_InitializeChainDisplays(struct PaletteTransition *p)
{
    const struct PaletteDisplayEntry *entry;
    struct DisplayAnimation *animation;
    s32 i;
    struct Position positionValue;
    struct Position *position;
    p->timer = 0;
    entry = p->entry;
    animation = p->animations;
    i = 0;
    position = &positionValue;
    for (; i < 5; i++) {
        s32 x = p->x;
        s32 offsetX = gUnk_086126CC[i * 2];
        s32 y = p->y;
        s32 offsetY = gUnk_086126CC[i * 2 + 1];
        s32 z = p->z;
        x += offsetX;
        y += offsetY;
        position->x = x;
        position->y = y;
        position->z = z;
        if (DisplayAnimation_InitializeSequence(animation, entry->id, entry->frames, 16, position) <
            0)
            return;
        DisplayAnimation_ClearFlags(animation, 1);
        if (!(entry->flags & 1))
            DisplayAnimation_SelectPaletteBank(animation, entry->palette);
        DisplayAnimation_SetPriority(animation, 2);
        Dir_Decompose(entry->directionIndex, &p->dirX, &p->dirY, p->direction);
        if (i == 0)
            DisplayAnimation_SetEntry(animation, gUnk_086126D6[i], p->dirX, p->dirY, 2, 0, 5);
        else
            DisplayAnimation_SetEntry(animation, gUnk_086126D6[i], 1, 0, 2, 0, 5);
        DisplayAnimation_UpdatePlayback(animation);
        animation++;
    }
}
