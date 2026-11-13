/* Headless mixer exerciser. This small test sequencer reads runtime MP2K song
 * data; the source port itself uses the game's existing m4a sequencer. LFO,
 * memory-access commands, extended commands and PSG notes are not interpreted
 * here. The tool reports skipped PSG notes instead of pretending full fidelity. */
#include "hw.h"
#include "gba/m4a_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *rom;
static size_t rom_size;
static struct SoundInfo sound;
static uint32_t voicegroup;
static unsigned tempo = 150, accumulator, notes, psg_notes;
static const uint8_t clocks[49] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    28,30,32,36,40,42,44,48,52,54,56,60,64,66,68,72,76,78,80,84,88,90,92,96
};

typedef struct Track {
    uint32_t pointer, stack[3];
    unsigned wait, depth, repeat, program, running, key, velocity, volume, bend_range;
    int keyshift, pan, bend, tune, on;
} Track;

typedef struct Voice {
    struct WaveData *wave;
    Track *track;
    unsigned gate, key;
    int pitch_key, pan;
} Voice;

static Track tracks[16];
static Voice voices[MAX_DIRECTSOUND_CHANNELS];

static uint8_t byte(uint32_t address)
{
    if (address < 0x08000000 || (size_t)(address - 0x08000000) >= rom_size) {
        fprintf(stderr, "Song address out of ROM: %08X\n", address); exit(1);
    }
    return rom[address - 0x08000000];
}
static uint32_t word(uint32_t a) { return (uint32_t)byte(a) | (uint32_t)byte(a+1)<<8 | (uint32_t)byte(a+2)<<16 | (uint32_t)byte(a+3)<<24; }
static void release(Track *t, int key)
{
    unsigned i;
    for (i=0; i<MAX_DIRECTSOUND_CHANNELS; ++i)
        if (voices[i].track == t && (key < 0 || voices[i].key == (unsigned)key)) sound.chans[i].statusFlags |= SOUND_CHANNEL_SF_STOP;
}

static void note(Track *t, unsigned command)
{
    unsigned gate = clocks[command - 0xcf], i, chosen = MAX_DIRECTSOUND_CHANNELS;
    uint32_t tone = voicegroup + t->program * 12;
    int pitch_key, pan = 0;
    if (byte(t->pointer) < 0x80) {
        t->key = byte(t->pointer++);
        if (byte(t->pointer) < 0x80) {
            t->velocity = byte(t->pointer++);
            if (byte(t->pointer) < 0x80) gate += byte(t->pointer++);
        }
    }
    if (command == 0xcf) gate = 0;
    pitch_key = (int)t->key;
    if (byte(tone) & 0x40) tone = word(tone + 4) + byte(word(tone + 8) + t->key) * 12;
    else if (byte(tone) & 0x80) {
        tone = word(tone + 4) + t->key * 12;
        pitch_key = byte(tone + 1);
        if (byte(tone + 3) & 0x80) pan = ((int)byte(tone + 3) - 0xc0) * 2;
    }
    if (byte(tone) & 7) { ++psg_notes; return; }
    for (i=0; i<MAX_DIRECTSOUND_CHANNELS; ++i) if (!sound.chans[i].statusFlags) { chosen=i; break; }
    if (chosen == MAX_DIRECTSOUND_CHANNELS) {
        for (i=0; i<MAX_DIRECTSOUND_CHANNELS; ++i) if (sound.chans[i].statusFlags & SOUND_CHANNEL_SF_STOP) { chosen=i; break; }
        if (chosen == MAX_DIRECTSOUND_CHANNELS) chosen = notes % MAX_DIRECTSOUND_CHANNELS;
    }
    struct SoundChannel *c = sound.chans + chosen;
    Voice *v = voices + chosen;
    uint32_t address = word(tone + 4), size = word(address + 12);
    if (!size || size > 0x400000 || address < 0x08000000 || (size_t)(address - 0x08000000) + 16 + size > rom_size) return;
    free(v->wave);
    v->wave = malloc(16 + size + 1);
    if (!v->wave) exit(2);
    memcpy(v->wave, rom + address - 0x08000000, 16 + size);
    v->wave->data[size] = 0;
    memset(c, 0, sizeof(*c));
    c->wav = v->wave; c->type = byte(tone);
    c->attack = byte(tone+8); c->decay = byte(tone+9); c->sustain = byte(tone+10); c->release = byte(tone+11);
    c->statusFlags = SOUND_CHANNEL_SF_START;
    v->track=t; v->gate=gate; v->key=t->key; v->pitch_key=pitch_key; v->pan=pan;
    ++notes;
}

static void step(Track *t)
{
    unsigned command = byte(t->pointer);
    if (command < 0x80) command = t->running;
    else { ++t->pointer; if (command >= 0xbd) t->running = command; }
    if (!command) { fprintf(stderr,"Invalid running command\n"); exit(1); }
    if (command >= 0xcf) { note(t,command); return; }
    if (command <= 0xb0) { t->wait = clocks[command-0x80]; return; }
    unsigned a = byte(t->pointer);
    switch (command) {
    case 0xb1: t->on=0; release(t,-1); break;
    case 0xb2: t->pointer=word(t->pointer); break;
    case 0xb3:
        if (t->depth<3) { t->stack[t->depth++]=t->pointer+4; t->pointer=word(t->pointer); }
        else t->pointer+=4;
        break;
    case 0xb4: if(t->depth) t->pointer=t->stack[--t->depth]; break;
    case 0xb5:
        if (!a || ++t->repeat<a) t->pointer=word(t->pointer+1);
        else { t->repeat=0; t->pointer+=5; }
        break;
    case 0xb9: t->pointer+=3; break;
    case 0xba: ++t->pointer; break;
    case 0xbb: tempo=a*2; ++t->pointer; break;
    case 0xbc: t->keyshift=(int8_t)a; ++t->pointer; break;
    case 0xbd: t->program=a; ++t->pointer; break;
    case 0xbe: t->volume=a; ++t->pointer; break;
    case 0xbf: t->pan=(int)a-64; ++t->pointer; break;
    case 0xc0: t->bend=(int)a-64; ++t->pointer; break;
    case 0xc1: t->bend_range=a; ++t->pointer; break;
    case 0xc2: case 0xc3: case 0xc4: case 0xc5: ++t->pointer; break;
    case 0xc8: t->tune=(int)a-64; ++t->pointer; break;
    case 0xcd: t->pointer+=2; break;
    case 0xce: if(a<0x80) { t->key=a; ++t->pointer; } release(t,(int)t->key); break;
    default: break;
    }
}

static void frame(unsigned count)
{
    unsigned i;
    accumulator += tempo;
    while (accumulator >= 150) {
        accumulator -= 150;
        for (i=0; i<MAX_DIRECTSOUND_CHANNELS; ++i)
            if (voices[i].gate && --voices[i].gate == 0) sound.chans[i].statusFlags |= SOUND_CHANNEL_SF_STOP;
        for (i=0; i<count; ++i) {
            Track *t=tracks+i;
            unsigned guard=0;
            while (t->on && !t->wait && ++guard<10000) step(t);
            if (guard==10000) { fprintf(stderr,"Song command budget exceeded\n"); exit(1); }
            if (t->wait) --t->wait;
        }
    }
    for (i=0; i<MAX_DIRECTSOUND_CHANNELS; ++i) {
        struct SoundChannel *c=sound.chans+i;
        Voice *v=voices+i;
        Track *t=v->track;
        if (!c->statusFlags || !t) continue;
        int pan = t->pan*2 + v->pan;
        if (pan<-128) pan=-128; if(pan>127) pan=127;
        c->rightVolume=(uint8_t)(((pan+128)*t->volume*2>>8)*t->velocity>>7);
        c->leftVolume=(uint8_t)(((127-pan)*t->volume*2>>8)*t->velocity>>7);
        double key = v->pitch_key + t->keyshift + (t->tune + t->bend*(int)t->bend_range)/64.0;
        c->frequency=(uint32_t)(c->wav->freq/1024.0 * pow(2.0,(key-60)/12));
    }
}

static void put16(FILE *f, unsigned v) { fputc(v&255,f); fputc((v>>8)&255,f); }
static void put32(FILE *f, uint32_t v) { put16(f,v&65535); put16(f,v>>16); }

int main(int argc, char **argv)
{
    unsigned song = argc>2 ? (unsigned)strtoul(argv[2],NULL,0) : 1;
    unsigned seconds = argc>4 ? (unsigned)strtoul(argv[4],NULL,0) : 10;
    FILE *input, *output;
    uint32_t header, samples = 0;
    int16_t pcm[352*2];
    unsigned count, i, f, peak=0;
    uint64_t energy=0;
    if(argc<4 || song>=1483 || !seconds || seconds>600) { fprintf(stderr,"usage: hw-song ROM SONG OUTPUT.wav [SECONDS, 1..600]\n"); return 2; }
    input=fopen(argv[1],"rb"); if(!input) return 2;
    if(fseek(input,0,SEEK_END)) return 2;
    long size=ftell(input); if(size<=0) return 2;
    rewind(input); rom_size=(size_t)size; rom=malloc(rom_size);
    if(!rom || fread(rom,1,rom_size,input)!=rom_size) return 2;
    fclose(input);
    header=word(0x0826456c+song*8); count=byte(header);
    if(!count || count>16) { fprintf(stderr,"Song has no supported tracks\n"); return 1; }
    voicegroup=word(header+4);
    for(i=0;i<count;++i) {
        tracks[i].pointer=word(header+8+i*4); tracks[i].on=1;
        tracks[i].volume=100; tracks[i].velocity=127; tracks[i].key=60; tracks[i].bend_range=2;
    }
    sound.maxChans=12; sound.masterVolume=15; sound.pcmFreq=21024; sound.divFreq=399;
    output=fopen(argv[3],"wb"); if(!output) return 2;
    fwrite("RIFF",1,4,output); put32(output,0); fwrite("WAVEfmt ",1,8,output); put32(output,16);
    put16(output,1); put16(output,2); put32(output,21024); put32(output,21024*4); put16(output,4); put16(output,16);
    fwrite("data",1,4,output); put32(output,0);
    for(f=0;f<(unsigned)(seconds*HW_CPU_HZ/(double)HW_CYCLES_PER_FRAME);++f) {
        frame(count);
        if(hw_mp2k_mix(&sound,pcm,352,21024)) return 1;
        for(i=0;i<352*2;++i) {
            int value=pcm[i]; unsigned amplitude=(unsigned)(value<0?-value:value);
            if(amplitude>peak) peak=amplitude;
            energy+=(uint64_t)((int64_t)value*value);
            put16(output,(uint16_t)value);
        }
        samples+=352;
    }
    fseek(output,4,SEEK_SET); put32(output,36+samples*4);
    fseek(output,40,SEEK_SET); put32(output,samples*4);
    if(fclose(output)) return 2;
    for(i=0;i<MAX_DIRECTSOUND_CHANNELS;++i) free(voices[i].wave);
    free(rom);
    printf("Song %u: %u PCM notes, %u skipped PSG notes; %u stereo samples, peak %u, RMS %.1f\n",song,notes,psg_notes,samples,peak,sqrt((double)energy/(samples*2)));
    if(!notes || !energy) { fprintf(stderr,"No audible PCM generated\n"); return 1; }
    return 0;
}
