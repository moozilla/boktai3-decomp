#ifndef GUARD_GAME_START_MENU_H
#define GUARD_GAME_START_MENU_H

#include "contact_swarm_sprite.h"
#include "display_sequence.h"
#include "scene_input_widget.h"

struct GameStart {
    u8 unk000[0x1C];
    struct SceneInputWidget background;
    struct SceneInputResource backgroundResource;
    u8 unk09C[8];
    struct SceneInputWidget decoration;
    struct SceneInputResource decorationResource;
    u8 unk124[8];
    struct SceneInputWidget title;
    struct SceneInputResource titleResource;
    u8 unk1AC[8];
    struct SceneInputResource itemResource;
    struct SceneInputWidget timeDigits[5];
    struct SceneInputWidget levelDigits[2];
    struct ContactSwarmSprite cursor;
    u8 cursorGraphics[0x1c];
    struct DisplaySequenceState cursorSequence;
    s32 active;
    u8 unk4D0[8];
    s32 timer;
    u8 unk4DC[8];
    s32 saveIndex;
    u8 unk4E8[4];
    void *cursorResource;
};

s32 GameStart_Init(struct GameStart *);
void GameStart_DrawSaveInfo(struct GameStart *);

typedef char GameStartSizeCheck[sizeof(struct GameStart) == 0x4f0 ? 1 : -1];

#endif
