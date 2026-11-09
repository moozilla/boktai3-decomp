void StatusScreen_DrawClock(struct StatusScreen *unused)
{
    int digits[2];
    s16 *dst;
    int row, col, n, base;
    u32 hours, minutes;
    struct StatusGameState *c = &gSunClock;
    if (!c)
        return;
    if (c->mode == 0 || c->mode == 4 || c->mode == 5) {
        dst = (s16 *)StatusScreen_GetTilePtr(0, 4, 15);
        hours = c->altCounters[0];
        minutes = c->altCounters[1];
    } else {
        dst = (s16 *)StatusScreen_GetTilePtr(0, 4, 15);
        hours = c->counters[0];
        minutes = c->counters[1];
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        base = 0x6a;
        for (col = 0; col < 4; col++)
            dst[col] = (shift + base + col) | -12288;
        dst += gBgRecords[0].width * 2;
    }

    dst = (s16 *)StatusScreen_GetTilePtr(0, 8, 15);
    if (c->mode == 0 || c->mode == 4 || c->mode == 5) {
        n = 2;
        base = 0x6e;
    } else {
        n = 2;
        base = 0x4e;
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        for (col = 0; col < n; col++)
            dst[col] = (shift + base + col) | -12288;
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(hours, 10);
    digits[1] = hours - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 16, 15);
    row = 0;
    for (; row < 2; row++) {
        int shift = row << 4;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = digits[col] + 0x60;
            *out++ = shift + tile | -12288;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(minutes, 10);
    digits[1] = minutes - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 22, 15);
    row = 0;
    for (; row < 2;) {
        int next = row + 1;
        int shift = row << 4;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = digits[col] + 0x60;
            *out++ = shift + tile | -12288;
        }
        dst += gBgRecords[0].width * 2;
        row = next;
    }
}
