// Trace real sprite submissions and the final OAM buffer before video upload.
#include "draw_trace.h"

#include <mgba/core/core.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    OAM_BYTES = 0x400,
    ENTRY_BYTES = 8,
    ENTRIES = OAM_BYTES / ENTRY_BYTES,
    MAX_FRAMES = 1801,
    REGULAR_DRAW = 0x08214600, // Sprite_QueueFramePieces
    ARM_DRAW = 0x030000E0,     // AgbMain copies the routine at 08000430 here
    FIRST_PROJECTED_DRAW = 0x08217F4C,
    SECOND_PROJECTED_DRAW = 0x08218254,
    THIRD_PROJECTED_DRAW = 0x08218508,
    FOURTH_PROJECTED_DRAW = 0x0821881C,
    RESET_QUEUE = 0x08215490,
    FINALIZE_QUEUE = 0x082154E4,
};

struct Submission {
    uint32_t pc, resume, drawable, tiles;
    uint16_t attr[3];
};

struct Queue {
    uint32_t count_address, buffer;
    struct Submission entries[ENTRIES];
};

struct Call {
    struct Submission source;
    uint32_t sp;
    unsigned first[2];
};

static struct mCore *core;
static FILE *queues, *sources, *uploads;
static unsigned generation, tile_offset;
static uint32_t dma_source, dma_destination;
static struct Queue direct, pending;
static struct Call calls[32];

_Noreturn static void fail(const char *problem)
{
    fprintf(stderr, "experiment sprite observer: %s\n", problem);
    exit(EXIT_FAILURE);
}

static FILE *output(const char *directory, const char *file, const char *mode)
{
    char path[1024];
    if (snprintf(path, sizeof(path), "%s/%s", directory, file) >= (int)sizeof(path))
        fail("output path too long");
    FILE *out = fopen(path, mode);
    if (!out)
        fail(file);
    return out;
}

bool draw_trace_command(struct mCore *attached, const char *directory, const char *line)
{
    if (strncmp(line, "drawtrace ", 10))
        return false;
    unsigned direct_count, direct_buffer, pending_count, pending_buffer;
    if (core || sscanf(line + 10, "%x %x %x %x %x", &direct_count, &direct_buffer,
                       &pending_count, &pending_buffer, &tile_offset) != 5)
        fail("invalid drawtrace declaration");
    core = attached;
    direct.count_address = direct_count;
    direct.buffer = direct_buffer;
    pending.count_address = pending_count;
    pending.buffer = pending_buffer;
    queues = output(directory, "oam-queues.bin", "wb");
    sources = output(directory, "sprite-sources.tsv", "w");
    uploads = output(directory, "oam-uploads.tsv", "w");
    fputs("generation\tframe\tpc\n", uploads);
    fputs("generation\tframe\tqueue\tindex\tattr0\tattr1\tattr2\tpc\tresume\tdrawable\ttiles\n", sources);
    return true;
}

bool draw_trace_active(void)
{
    return core != NULL;
}

static uint16_t read16(uint32_t address)
{
    return core->rawRead16(core, address, -1);
}

static uint32_t read32(uint32_t address)
{
    return core->rawRead32(core, address, -1);
}

static uint32_t read_pointer(uint32_t address)
{
    if ((address >= 0x02000000 && address <= 0x0203FFFC) ||
        (address >= 0x03000000 && address <= 0x03007FFC))
        return read32(address);
    return 0;
}

static unsigned count(const struct Queue *queue)
{
    unsigned value = read32(queue->count_address);
    return value < ENTRIES ? value : ENTRIES;
}

static void submissions(struct Queue *queue, unsigned first, const struct Submission *source)
{
    unsigned end = count(queue);
    for (unsigned i = first; i < end; i++) {
        struct Submission *slot = &queue->entries[i];
        *slot = *source;
        for (unsigned j = 0; j < 3; j++)
            slot->attr[j] = read16(queue->buffer + i * ENTRY_BYTES + 2 * j);
    }
}

static void write_sources(const struct Queue *queue, char name)
{
    unsigned length = count(queue);
    unsigned frame = core->frameCounter(core);
    for (unsigned i = 0; i < length; i++) {
        struct Submission item = queue->entries[i];
        unsigned address = queue->buffer + i * ENTRY_BYTES;
        if (name == 'p') {
            // Pending entries gain depth links during sort. The three video
            // attributes identify the original submission independently of those.
            uint16_t actual[3] = {read16(address), read16(address + 2), read16(address + 4)};
            if (memcmp(item.attr, actual, sizeof(actual))) {
                item = (struct Submission){0};
                memcpy(item.attr, actual, sizeof(actual));
            }
        }
        fprintf(sources, "%u\t%u\t%c\t%u\t%04X\t%04X\t%04X\t%08" PRIX32 "\t%08" PRIX32
                "\t%08" PRIX32 "\t%08" PRIX32 "\n", generation, frame, name, i,
                item.attr[0], item.attr[1], item.attr[2], item.pc,
                item.resume, item.drawable, item.tiles);
    }
}

static void finalize(void)
{
    if (generation >= MAX_FRAMES)
        fail("too many sprite queue generations");
    uint8_t bytes[OAM_BYTES + 4];
    uint32_t frame = core->frameCounter(core);
    for (unsigned i = 0; i < 4; i++)
        bytes[i] = frame >> (i * 8);
    for (unsigned i = 0; i < OAM_BYTES; i += 2) {
        unsigned halfword = read16(direct.buffer + i);
        bytes[i + 4] = halfword;
        bytes[i + 5] = halfword >> 8;
    }
    if (fwrite(bytes, sizeof(bytes), 1, queues) != 1)
        fail("cannot write final OAM queue");
    write_sources(&direct, 'd');
    write_sources(&pending, 'p');
    generation++;
}

void draw_trace_step(uint32_t pc, uint32_t sp, uint32_t lr, uint32_t arg0, unsigned mode)
{
    struct Call *call = &calls[mode & 31];
    if (call->source.pc && (sp > call->sp || (sp == call->sp && pc == call->source.resume))) {
        if (sp == call->sp) {
            if (call->source.pc == FINALIZE_QUEUE) {
                finalize();
            } else {
                submissions(&direct, call->first[0], &call->source);
                submissions(&pending, call->first[1], &call->source);
            }
        }
        call->source.pc = 0;
    }
    if (pc == RESET_QUEUE) {
        memset(direct.entries, 0, sizeof(direct.entries));
        memset(pending.entries, 0, sizeof(pending.entries));
        return;
    }
    bool instance = pc == ARM_DRAW || pc == REGULAR_DRAW;
    if (!instance && pc != FINALIZE_QUEUE && pc != FIRST_PROJECTED_DRAW &&
        pc != SECOND_PROJECTED_DRAW && pc != THIRD_PROJECTED_DRAW &&
        pc != FOURTH_PROJECTED_DRAW)
        return;
    uint32_t tiles = pc == ARM_DRAW ? read_pointer(arg0 + tile_offset) : 0;
    call->source = (struct Submission){ .pc = pc, .resume = lr & ~1u,
                                       .drawable = instance ? arg0 : 0, .tiles = tiles };
    call->sp = sp;
    call->first[0] = count(&direct);
    call->first[1] = count(&pending);
}

void draw_trace_dma(uint32_t address, uint32_t value, uint32_t pc)
{
    if (address == 0x040000D4) {
        dma_source = value;
    } else if (address == 0x040000D8) {
        dma_destination = value;
    } else if (address == 0x040000DC && (value & 0x80000000)) {
        if (dma_destination == 0x07000000) {
            // The upload task initiates an immediate 0x400-byte copy.
            bool known = dma_source == direct.buffer && value == 0x84000100 && generation;
            fprintf(uploads, "%d\t%u\t%08" PRIX32 "\n",
                    known ? (int)generation - 1 : -1, core->frameCounter(core), pc);
        }
    }
}

void draw_trace_close(void)
{
    if (!core)
        return;
    int a = fclose(queues), b = fclose(sources), c = fclose(uploads);
    if (a || b || c)
        fail("could not finish the sprite evidence");
}
