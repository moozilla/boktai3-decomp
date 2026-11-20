// Optional, CPU-only store observation for the headless mGBA harness.
#include "observe.h"

#include <mgba/core/core.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_FIELDS = 16, MAX_OFFSETS = 8, MAX_EVENTS = 250000, MAX_CONTEXT = 24 };
struct Watch {
    uint32_t root, offsets[MAX_OFFSETS];
    uint32_t address, links[MAX_OFFSETS], old_value;
    unsigned width, depth;
};

struct Entry {
    uint32_t pc, sp, resume;
};
struct Context {
    struct Entry entries[MAX_CONTEXT];
    unsigned depth;
};
static struct Context contexts[32], *active_context;
static struct mCore *core;
static const char *outdir;
static FILE *events;
static struct Watch fields[MAX_FIELDS];
static unsigned nfields, sequence;
static unsigned intervention_field = MAX_FIELDS;
static uint32_t touched, low, high;

static void stop(const char *reason)
{
    fprintf(stderr, "experiment observer: %s\n", reason);
    exit(EXIT_FAILURE);
}

void observe_attach(struct mCore *attached, const char *directory)
{
    core = attached;
    outdir = directory;
}

static void open_events(void)
{
    if (events)
        return;
    char path[1024];
    if (snprintf(path, sizeof(path), "%s/observations.tsv", outdir) >= (int)sizeof(path))
        stop("output path too long");
    events = fopen(path, "w");
    if (!events)
        stop("cannot open observations.tsv");
    fputs("kind\tsequence\tframe\tfield\tlabel\taddress\tvalue\tbefore\tpc\tcontext\n", events);
}

static void count_event(void)
{
    if (++sequence > MAX_EVENTS)
        stop("too many observed stores; narrow the replay or field selection");
}

static bool ram(uint32_t address, unsigned width)
{
    return (address >= 0x02000000 && address <= 0x02040000 - width) ||
           (address >= 0x03000000 && address <= 0x03008000 - width);
}

static uint32_t raw(uint32_t address, unsigned width)
{
    uint32_t value = 0;
    for (unsigned i = 0; i < width; ++i)
        value |= (uint32_t)core->rawRead8(core, address + i, -1) << (8 * i);
    return value;
}

static void resolve(struct Watch *watch)
{
    uint32_t address = watch->root;
    memset(watch->links, 0, sizeof(watch->links));
    watch->address = 0;
    for (unsigned i = 0; i < watch->depth; ++i) {
        address += watch->offsets[i];
        if (i + 1 == watch->depth) {
            if (ram(address, watch->width))
                watch->address = address;
            return;
        }
        if (!ram(address, 4))
            return;
        watch->links[i] = address;
        address = raw(address, 4);
        if (!ram(address, 1))
            return;
    }
}

static void extend_range(uint32_t address, unsigned width)
{
    if (!address)
        return;
    if (address < low)
        low = address;
    if (address + width > high)
        high = address + width;
}

static void recalculate_bounds(void)
{
    low = UINT32_MAX;
    high = 0;
    for (unsigned i = 0; i < nfields; ++i) {
        struct Watch *watch = &fields[i];
        extend_range(watch->address, watch->width);
        for (unsigned j = 0; j + 1 < watch->depth; ++j)
            extend_range(watch->links[j], 4);
    }
}

static bool overlaps(uint32_t a, unsigned width, uint32_t b, unsigned size)
{
    return b && a < b + size && b < a + width;
}

bool observe_before_store(uint32_t address, unsigned width)
{
    if (!nfields || address + width <= low || address >= high)
        return false;
    touched = 0;
    for (unsigned i = 0; i < nfields; ++i) {
        struct Watch *watch = &fields[i];
        bool relevant = overlaps(address, width, watch->address, watch->width);
        for (unsigned j = 0; !relevant && j + 1 < watch->depth; ++j)
            relevant = overlaps(address, width, watch->links[j], 4);
        if (relevant) {
            watch->old_value = watch->address ? raw(watch->address, watch->width) : 0;
            touched |= 1u << i;
        }
    }
    return touched != 0;
}

// Tracks named probe entries by their observed return address and stack depth.
// Separate CPU modes keep IRQ activity from looking like a game function call.
void observe_instruction(uint32_t pc, uint32_t sp, uint32_t lr, unsigned mode, bool entered)
{
    if (!nfields)
        return;
    struct Context *stack = &contexts[mode & 31];
    active_context = stack;
    while (stack->depth) {
        struct Entry *top = &stack->entries[stack->depth - 1];
        if (sp > top->sp || (sp == top->sp && pc == top->resume))
            --stack->depth;
        else
            break;
    }
    uint32_t resume = lr & ~1u;
    if (!entered || !ram(sp, 4) || resume == pc ||
        (resume < 0x08000000 && !ram(resume, 2)) || resume >= 0x0A000000)
        return;
    if (stack->depth) {
        struct Entry *top = &stack->entries[stack->depth - 1];
        if (top->sp == sp && top->resume == resume)
            --stack->depth; // tail transition with the same return
    }
    if (stack->depth == MAX_CONTEXT) {
        memmove(stack->entries, stack->entries + 1, (MAX_CONTEXT - 1) * sizeof(*stack->entries));
        --stack->depth;
    }
    stack->entries[stack->depth++] = (struct Entry){ pc, sp, resume };
}

static void end_store_line(bool intervention)
{
    unsigned depth = !intervention && active_context ? active_context->depth : 0;
    for (unsigned i = 0; i < depth; ++i) {
        const struct Entry *entry = &active_context->entries[i];
        fprintf(events, "%s%08" PRIX32 ":%08" PRIX32, i ? "," : "", entry->pc, entry->resume);
    }
    fputc('\n', events);
}

void observe_after_store(uint32_t pc)
{
    bool intervention = intervention_field < MAX_FIELDS;
    if (intervention)
        pc = 0;
    for (unsigned i = 0; i < nfields; ++i) {
        if (!(touched & (1u << i)))
            continue;
        struct Watch *watch = &fields[i];
        uint32_t previous = watch->address;
        resolve(watch);
        uint32_t value = watch->address ? raw(watch->address, watch->width) : 0;
        unsigned frame = core->frameCounter(core);
        if (previous != watch->address) {
            count_event();
            fprintf(events, "%s\t%u\t%u\t%u\t-\t%08" PRIX32 "\t%08" PRIX32
                    "\t%08" PRIX32 "\t%08" PRIX32 "\t", intervention ? "intervention_bind" : "bind", sequence, frame, i,
                    watch->address, value, previous, pc);
            end_store_line(intervention);
        } else if (watch->address && (value != watch->old_value || i == intervention_field)) {
            count_event();
            fprintf(events, "%s\t%u\t%u\t%u\t-\t%08" PRIX32 "\t%08" PRIX32
                    "\t%08" PRIX32 "\t%08" PRIX32 "\t", intervention ? "intervention" : "write", sequence, frame, i,
                    watch->address, value, watch->old_value, pc);
            end_store_line(intervention);
        }
    }
    recalculate_bounds();
}

bool observe_command(const char *line)
{
    if (!strncmp(line, "observe ", 8)) {
        if (nfields == MAX_FIELDS)
            stop("too many fields");
        unsigned root, width, consumed;
        int n;
        if (sscanf(line + 8, "%x %u%n", &root, &width, &n) != 2 ||
            (width != 1 && width != 2 && width != 4) || !ram(root, 1))
            stop("invalid field declaration");
        consumed = 8 + n;
        struct Watch *watch = &fields[nfields++];
        watch->root = root;
        watch->width = width;
        unsigned offset;
        while (sscanf(line + consumed, "%x%n", &offset, &n) == 1) {
            if (watch->depth == MAX_OFFSETS || offset > 0x40000)
                stop("invalid field offset");
            watch->offsets[watch->depth++] = offset;
            consumed += n;
        }
        if (!watch->depth)
            stop("field needs an offset (zero is valid)");
        open_events();
        resolve(watch);
        recalculate_bounds();
        return true;
    }
    if (!strncmp(line, "setfield ", 9)) {
        unsigned index, value;
        if (sscanf(line + 9, "%u %x", &index, &value) != 2 || index >= nfields)
            stop("invalid named-field intervention");
        struct Watch *watch = &fields[index];
        resolve(watch);
        recalculate_bounds();
        if (!watch->address)
            stop("the intervention's named field is unbound at this frame");
        if (watch->width < 4 && value >= (1u << (8 * watch->width)))
            stop("the intervention value does not fit its field");
        // The memory callbacks see this store just like CPU stores. Mark it
        // as authored input so the current PC is never reported as its writer.
        intervention_field = index;
        if (watch->width == 1)
            core->busWrite8(core, watch->address, value);
        else if (watch->width == 2)
            core->busWrite16(core, watch->address, value);
        else
            core->busWrite32(core, watch->address, value);
        intervention_field = MAX_FIELDS;
        return true;
    }
    if (!strncmp(line, "sample ", 7)) {
        char name[48];
        if (sscanf(line + 7, "%47[A-Za-z0-9_-]", name) != 1)
            stop("invalid sample label");
        open_events();
        for (unsigned i = 0; i < nfields; ++i) {
            struct Watch *watch = &fields[i];
            resolve(watch);
            uint32_t value = watch->address ? raw(watch->address, watch->width) : 0;
            count_event();
            fprintf(events, "sample\t%u\t%u\t%u\t%s\t%08" PRIX32 "\t%08" PRIX32
                    "\t-\t-\t\n", sequence, core->frameCounter(core), i, name,
                    watch->address, value);
        }
        recalculate_bounds();
        return true;
    }
    return false;
}

void observe_close(void)
{
    if (events && fclose(events))
        stop("could not finish observations.tsv");
}
