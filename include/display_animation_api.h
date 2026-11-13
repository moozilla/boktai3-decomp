#ifndef GUARD_DISPLAY_ANIMATION_API_H
#define GUARD_DISPLAY_ANIMATION_API_H

#include "display_animation.h"

s32 DisplayAnimation_InitializeSequence(struct DisplayAnimation *, s32, u32, s32,
                                        const struct DisplayPosition *);
s32 DisplayAnimation_InitializeKeyframes(struct DisplayAnimation *, u32, s32,
                                         const struct DisplayPosition *);
u32 DisplayAnimation_GetTileIndex(struct DisplayAnimation *);
s32 DisplayAnimation_SetPosition(struct DisplayAnimation *, const struct DisplayPosition *);
s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *);
s32 DisplayAnimation_SelectPaletteBank(struct DisplayAnimation *, u32);
s32 DisplayAnimation_SetEntry(struct DisplayAnimation *, u32, u32, u32, u32, u32, u32);
s32 DisplayAnimation_Destroy(struct DisplayAnimation *);

#endif
