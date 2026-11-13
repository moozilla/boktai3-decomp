/* Portable counterpart of the MP2K IWRAM mixer. */
#include "internal.h"
#include "gba/m4a_internal.h"

#if UINTPTR_MAX == UINT32_MAX
_Static_assert(sizeof(struct SoundChannel) == 0x40, "MP2K channel layout");
_Static_assert(offsetof(struct SoundInfo, chans) == 0x50, "MP2K channels offset");
_Static_assert(offsetof(struct SoundInfo, pcmBuffer) == 0x350, "MP2K PCM offset");
#endif

static int16_t clip16(int32_t value)
{
    return (int16_t)(value > 32767 ? 32767 : value < -32768 ? -32768 : value);
}

static int envelope(struct SoundChannel *c)
{
    unsigned volume = c->envelopeVolume;
    if (!(c->statusFlags & SOUND_CHANNEL_SF_ON)) return 0;
    if (c->statusFlags & SOUND_CHANNEL_SF_START) {
        if ((c->statusFlags & SOUND_CHANNEL_SF_STOP) || !c->wav || !c->wav->size) goto stop;
        c->statusFlags = SOUND_CHANNEL_SF_ENV_ATTACK;
        if (c->wav->status & 0xc000) c->statusFlags |= SOUND_CHANNEL_SF_LOOP;
        c->currentPointer = c->wav->data;
        c->count = c->wav->size;
        c->fw = 0;
        volume = c->attack;
        if (volume >= 255) { volume = 255; --c->statusFlags; }
    } else if (c->statusFlags & SOUND_CHANNEL_SF_IEC) {
        if (!c->pseudoEchoLength || --c->pseudoEchoLength == 0) goto stop;
    } else if (c->statusFlags & SOUND_CHANNEL_SF_STOP) {
        volume = volume * c->release >> 8;
        if (volume <= c->pseudoEchoVolume) {
            volume = c->pseudoEchoVolume;
            if (!volume) goto stop;
            c->statusFlags |= SOUND_CHANNEL_SF_IEC;
        }
    } else if ((c->statusFlags & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_DECAY) {
        volume = volume * c->decay >> 8;
        if (volume <= c->sustain) {
            volume = c->sustain;
            if (volume) --c->statusFlags;
            else {
                volume = c->pseudoEchoVolume;
                if (!volume) goto stop;
                c->statusFlags |= SOUND_CHANNEL_SF_IEC;
            }
        }
    } else if ((c->statusFlags & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_ATTACK) {
        volume += c->attack;
        if (volume >= 255) { volume = 255; --c->statusFlags; }
    }
    c->envelopeVolume = (uint8_t)volume;
    return 1;
stop:
    c->statusFlags = 0;
    return 0;
}

static int sample_channel(struct SoundChannel *c, uint32_t step, int32_t *sample)
{
    const struct WaveData *wave = c->wav;
    uint32_t loop_length;
    int32_t first, next;
    uint64_t position;
    uint32_t advance;
    if (!wave || !c->currentPointer || !c->count) return 0;
    first = *c->currentPointer;
    if (c->type & TONEDATA_TYPE_FIX) {
        *sample = first;
        advance = 1;
    } else {
        if (c->count > 1) next = c->currentPointer[1];
        else if ((c->statusFlags & SOUND_CHANNEL_SF_LOOP) && wave->loopStart < wave->size) next = wave->data[wave->loopStart];
        else next = 0;
        /* Explicit division implements an arithmetic shift for negative deltas. */
        int64_t product = (int64_t)(next - first) * (c->fw & 0x7fffff);
        int32_t fraction = product >= 0 ? (int32_t)(product / 0x800000) : (int32_t)-((-product + 0x7fffff) / 0x800000);
        *sample = first + fraction;
        position = (uint64_t)(c->fw & 0x7fffff) + step;
        c->fw = (uint32_t)position & 0x7fffff;
        advance = (uint32_t)(position >> 23);
    }
    if (advance < c->count) {
        c->currentPointer += advance;
        c->count -= advance;
        return 1;
    }
    advance -= c->count;
    loop_length = wave->loopStart < wave->size ? wave->size - wave->loopStart : 0;
    if ((c->statusFlags & SOUND_CHANNEL_SF_LOOP) && loop_length) {
        advance %= loop_length;
        c->currentPointer = (s8 *)wave->data + wave->loopStart + advance;
        c->count = loop_length - advance;
    } else { c->count = 0; c->statusFlags = 0; }
    return 1;
}

int hw_mp2k_mix(struct SoundInfo *sound, int16_t *stereo, size_t frames, uint32_t sample_rate)
{
    unsigned channel, channels;
    size_t i;
    int32_t mixed[PCM_DMA_BUF_SIZE * 2];
    if (!sound || !stereo || !sample_rate || frames > PCM_DMA_BUF_SIZE) return HW_BAD_DATA;
    hw_zero(mixed, frames * 2 * sizeof(*mixed));
    channels = sound->maxChans > MAX_DIRECTSOUND_CHANNELS ? MAX_DIRECTSOUND_CHANNELS : sound->maxChans;
    /* Reverb's original ring has two adjacent blocks and both stereo sides. */
    size_t offset = 0;
    if (sound->pcmDmaCounter > 1 && sound->pcmDmaCounter <= sound->pcmDmaPeriod)
        offset = (sound->pcmDmaPeriod - (sound->pcmDmaCounter - 1)) * frames;
    if (offset + frames > PCM_DMA_BUF_SIZE) return HW_BAD_DATA;
    if (sound->reverb) for (i = 0; i < frames; ++i) {
        size_t next = (offset + frames + i) % PCM_DMA_BUF_SIZE;
        int32_t sum = sound->pcmBuffer[offset + i] + sound->pcmBuffer[offset + i + PCM_DMA_BUF_SIZE] +
                      sound->pcmBuffer[next] + sound->pcmBuffer[next + PCM_DMA_BUF_SIZE];
        int32_t feedback = sum * sound->reverb / 512;
        mixed[i * 2] = mixed[i * 2 + 1] = feedback * 256;
    }
    for (channel = 0; channel < channels; ++channel) {
        struct SoundChannel *c = sound->chans + channel;
        uint32_t step;
        unsigned level;
        if (!envelope(c)) continue;
        level = ((sound->masterVolume + 1) * c->envelopeVolume) >> 4;
        c->envelopeVolumeRight = (uint8_t)(level * c->rightVolume >> 8);
        c->envelopeVolumeLeft = (uint8_t)(level * c->leftVolume >> 8);
        step = sample_rate == (uint32_t)sound->pcmFreq && sound->divFreq > 0 ?
               (uint32_t)((uint64_t)(uint32_t)sound->divFreq * c->frequency) :
               (uint32_t)(((uint64_t)c->frequency << 23) / sample_rate);
        for (i = 0; i < frames; ++i) {
            int32_t sample;
            if (!sample_channel(c, step, &sample)) break;
            mixed[2 * i] += sample * c->envelopeVolumeLeft;
            mixed[2 * i + 1] += sample * c->envelopeVolumeRight;
        }
    }
    for (i = 0; i < frames; ++i) {
        stereo[i * 2] = clip16(mixed[i * 2]);
        stereo[i * 2 + 1] = clip16(mixed[i * 2 + 1]);
        sound->pcmBuffer[offset + i] = (s8)(stereo[i * 2 + 1] / 256);
        sound->pcmBuffer[offset + i + PCM_DMA_BUF_SIZE] = (s8)(stereo[i * 2] / 256);
    }
    return HW_OK;
}

void hw_m4a_attach(Hw *hw, struct SoundInfo *sound) { hw->sound = sound; }

void SoundMain(void)
{
    Hw *hw = hw_bound();
    struct SoundInfo *sound;
    if (!hw) return;
    sound = hw->sound;
    if (!sound) {
        uint8_t *slot = hw_map(hw, 0x03007ff0, 4);
        if (slot) sound = (struct SoundInfo *)hw_map(hw, hw_get32(slot), sizeof(*sound));
    }
    if (!sound || sound->ident != ID_NUMBER || sound->pcmSamplesPerVBlank <= 0 || sound->pcmSamplesPerVBlank > PCM_DMA_BUF_SIZE || sound->pcmFreq <= 0) return;
    ++sound->ident;
    if (sound->MPlayMainHead) sound->MPlayMainHead(sound->musicPlayerHead);
    if (sound->CgbSound) sound->CgbSound();
    hw_sync_io(hw);
    hw->audio_frames = (size_t)sound->pcmSamplesPerVBlank;
    hw->audio_rate = (uint32_t)sound->pcmFreq;
    if (hw_mp2k_mix(sound, hw->audio_buffer, hw->audio_frames, hw->audio_rate) != HW_OK) hw->audio_frames = 0;
    hw_psg_render(hw, hw->audio_buffer, hw->audio_frames, hw->audio_rate, 1);
    sound->ident = ID_NUMBER;
    if (hw->host.audio && hw->audio_frames) hw->host.audio(hw->host.user, hw->audio_buffer, hw->audio_frames, hw->audio_rate);
}

void SoundMainBTM(void) { SoundMain(); }
