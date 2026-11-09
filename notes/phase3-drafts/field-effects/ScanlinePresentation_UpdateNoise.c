void ScanlinePresentation_UpdateNoise(struct ScanlinePresentation *p)
{
    s32 i;
    s16 value;
    s16 *amplitude, *delta;
    for (i = 0; i < p->start; i++)
        p->rows[p->active][i] = 0;
    i = p->start;
    amplitude = &p->amplitude;
    delta = &p->delta;
    for (; i < p->end; i++) {
        s32 v = (ScanlinePresentation_NextNoiseRandom() * p->amplitude) >> 18;
        p->rows[p->active][i] = v | (v << 8);
    }
    for (i = p->end; i < 160; i++)
        p->rows[p->active][i] = 0;
    value = (*amplitude += *delta);
    if (value < 0)
        *amplitude = 0;
    else if (value > 128)
        *amplitude = 128;
}
