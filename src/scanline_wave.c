#include "scanline_wave.h"

int ScanlineWave_Update(struct ScanlineWave *wave)
{
    if (wave->enabled) {
        ScanlineEffect_SetSource(
            wave->target, wave->mode,
            &wave->samples[wave->bank][(gBgRecords[gScanlineEffectLayer].y + wave->phase) & 127]);
        if (++wave->timer >= wave->period) {
            wave->timer = 0;
            wave->phase += wave->step;
        }
    }
    return 0;
}

u32 ScanlineWave_Destroy(void)
{
    sub_082194D4();
    gScanlineWave = 0;
    return 0;
}

int ScanlineWave_Initialize(struct ScanlineWave *wave, int id)
{
    int amplitude;
    int i, j;
    int limit;
    int wrap;
    const s16 *table;
    int mask;
    gScanlineWave = (u32)wave;
    wave->id = id;
    wave->target = Script_GetKeywordValue('t', 0);
    wave->mode = Script_GetKeywordValue('m', 0);
    if (Script_SeekToKeyword('r')) {
        wave->start = Script_GetValue();
        wave->end = Script_GetValue();
    } else {
        wave->start = 0;
        wave->end = 160;
    }
    amplitude = Script_GetKeywordValue('s', 8);
    wave->timer = 0;
    wave->step = Script_GetKeywordValue('o', 2);
    wave->phase = 0;
    wave->period = Script_GetKeywordValue('i', 5);
    wave->bank = 0;
    wave->enabled = 1;
    i = 0;
    limit = 287;
    wrap = 127;
    table = gUnk_086149C4;
    mask = 255;
    do {
        j = 0;
        do {
            int v = (table[(j & wrap) & mask] * amplitude) >> 12;
            wave->samples[i][j] = v;
            j++;
        } while (j <= limit);
        i++;
    } while (i <= 0);
}

struct ScanlineWave *ScanlineWave_Create(int id)
{
    struct ScanlineWave *wave;
    if (gScanlineWave)
        return (void *)gScanlineWave;
    if (Script_GetKeywordValue('a', 0))
        wave = Task_Create(11, sizeof(*wave));
    else
        wave = Task_Create(9, sizeof(*wave));
    if (wave) {
        Task_SetCallbacks(wave, (void *)ScanlineWave_Update, (void *)ScanlineWave_Destroy);
        if (ScanlineWave_Initialize(wave, (u16)id) < 0) {
            Task_Kill(wave);
            return 0;
        }
    }
    return wave;
}
