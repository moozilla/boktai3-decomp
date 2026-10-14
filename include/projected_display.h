#ifndef GUARD_PROJECTED_DISPLAY_H
#define GUARD_PROJECTED_DISPLAY_H
#include "contact_actor.h"
struct ProjectedDisplayResource {
    u32 words[8];
};
/* Observed 0x60-byte animation entry; ownership/other fields remain tentative. */
struct ProjectedDisplayEntry {
    u32 unknown0;
    u8 registered, unknown5[3];
    u32 flags;
    u8 unknownC[0x14];
    struct ContactPosition position;
    u8 unknown28[0x12];
    u16 paletteIndex;
    u8 unknown3C[0xC];
    const void *paletteData;
    u8 unknown4C[0x14];
};
struct ProjectedDisplay {
    u8 task[0x18];
    s32 height;
    u32 unknown1C;
    struct ContactPosition world, raised, screen;
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry entries[5];
};
extern const s32 gUnk_0860594C[];
static inline s32 projectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *, s32);
void ProjectedDisplay_UpdateEntries(struct ProjectedDisplay *);
s32 ProjectedDisplay_InitializeEntries(struct ProjectedDisplay *);
void ProjectedDisplay_RemoveEntries(struct ProjectedDisplay *);
void ProjectedDisplay_UpdateScreenPosition(struct ProjectedDisplay *);
s32 ProjectedDisplay_Update(struct ProjectedDisplay *);
s32 ProjectedDisplay_Destroy(struct ProjectedDisplay *);
s32 ProjectedDisplay_Initialize(struct ProjectedDisplay *, const struct ContactPosition *);
struct ProjectedDisplay *ProjectedDisplay_Create(const struct ContactPosition *);
u32 ProjectedDisplay_GetHeight(struct ProjectedDisplay *);
u32 ProjectedDisplay_SetHeight(struct ProjectedDisplay *, s32);
#endif
