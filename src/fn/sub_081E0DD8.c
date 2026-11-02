// MATCHING_CONCESSION: register-pinning
#include "game_start_menu.h"
#include "display_sprite_playback.h"
#include "game_state.h"
#include "menu_parameters.h"

struct GameStartSeqs {
    u32 seq[3];
};
extern const struct GameStartSeqs gUnk_0824FD50;
extern const u8 gUnk_0824FD5C[];
extern const u8 gUnk_0824FDBC[];

void *memcpy(void *, const void *, u32);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *PlayerName_GetBuffer(void);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(u32, u32, const u8 *);
void Anim_SetSequence(void *, void *, u16, u32, u32);
void Bg_SetTilemapEntry(s32, s32, s32, s32, s32, s32);
int Script_ParseStringRef(const void *);
int SceneSequence_GetSelectedMessage(void);
const u8 *Text_LookupString(int);
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);

/* Draws the name, player portrait and title, play time, and level from a save file. */
void GameStart_DrawSaveInfo(struct GameStart *p)
{
    u8 text[0x20];
    struct GameStartSeqs seqs;
    /* Solar Boy, Dark Boy, and Vampire Boy each occupy two 16-tile strips. */
    u8 titleTiles[6][16];
    s32 i;
    const u8 *s;
    struct SceneInputResource *levelRes;

    seqs = gUnk_0824FD50;
    memcpy(titleTiles, gUnk_0824FD5C, sizeof(titleTiles));
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    if ((s16)gGameState->flag86E != 0) {
        u8 misc[10];
        u32 zero;
        u32 *zp;
        s32 idx;
        s32 hours, minutes, level;
        u8 *q;
        struct SceneInputWidget *tens;
        struct SceneInputResource *res;
        struct SceneInputWidget *minuteOnes, *minuteTens, *colon, *levelOnes, *hourOnes;

        memcpy(misc, gUnk_0824FDBC, sizeof(misc));
        s = PlayerName_GetBuffer();
        Menu_EraseRect(8, 7, 12, 2);
        Menu_DrawText(8, 7, s);
        {
            /* The game reads player form here, then displays the first portrait and title. */
            s32 playerForm = gGameState->form;
            asm("" : : "r"(playerForm));
        }
        idx = 0;
        Anim_SetSequence(&p->cursorSequence, p->cursorResource, seqs.seq[idx], 0, 0);
        PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)(&p->cursor),
                                    (struct DisplaySequenceState *)(&p->cursorSequence));
        p->cursor.flags &= ~1;
        for (i = 0; i <= 11; i++) {
            Bg_SetTilemapEntry(3, i + 8, 6, titleTiles[idx][i], 0, 2);
            zp = &zero;
            hourOnes = &p->timeDigits[1];
            minuteOnes = &p->timeDigits[4];
            minuteTens = &p->timeDigits[3];
            colon = &p->timeDigits[2];
            levelOnes = &p->levelDigits[1];
        }
        Menu_EraseRect(3, 10, 24, 2);
        *zp = 0;
        CpuSet(zp, text, 0x05000008);
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.aBlock) +
                              SceneSequence_GetSelectedMessage());
        q = text;
        while (*s)
            *q++ = *s++;
        Menu_DrawText(3, 10, text);
        if (gGameState->lap > 0)
            p->decoration.flags &= ~1;

        tens = &p->timeDigits[0];
        hours = Div(gGameState->unk614, 0x34bc0);
        minutes = Div(gGameState->unk614, 0xe10) - hours * 60;
        if (hours > 99) {
            hours = 99;
            minutes = 59;
        }
        res = &p->itemResource;
        Anim_LoadKeyframe(hourOnes, res, (u16)Mod(hours, 10), 0);
        hours = Div(hours, 10);
        Anim_LoadKeyframe(tens, res, (u16)Mod(hours, 10), 0);
        tens->flags &= ~1;
        tens[1].flags &= ~1;
        Anim_LoadKeyframe(minuteOnes, res, (u16)Mod(minutes, 10), 0);
        minutes = Div(minutes, 10);
        Anim_LoadKeyframe(minuteTens, res, (u16)Mod(minutes, 10), 0);
        minuteTens->flags &= ~1;
        minuteOnes->flags &= ~1;
        colon->flags &= ~1;

        if (gGameState->form == 6) {
            level = gGameState->level + 10;
            if (level > 99)
                level = 99;
        } else {
            level = gGameState->level;
        }
        tens = &p->levelDigits[0];
        levelRes = &p->itemResource;
        Anim_LoadKeyframe(levelOnes, levelRes, (u16)Mod(level, 10), 0);
        level = Div(level, 10);
        Anim_LoadKeyframe(tens, levelRes, (u16)Mod(level, 10), 0);
        tens->flags &= ~1;
        tens[1].flags &= ~1;
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 3);
        Menu_EraseRect(0x15, 7, 3, 2);
        Menu_DrawText(0x15, 7, s);
    } else {
        for (i = 0; i <= 8; i++)
            Bg_SetTilemapEntry(3, i + 8, 6, 1, 0, 2);
        p->cursor.flags |= 1;
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 2);
        Menu_EraseRect(8, 6, 0x12, 2);
        Menu_DrawText(8, 6, s);
    }
    s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 9);
    Menu_EraseRect(3, 0xe, 0x18, 2);
    Menu_DrawText(3, 0xe, s);
}
