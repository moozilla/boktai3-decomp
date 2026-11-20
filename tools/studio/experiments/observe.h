#ifndef STUDIO_EXPERIMENT_OBSERVE_H
#define STUDIO_EXPERIMENT_OBSERVE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef STUDIO_EXPERIMENTS
struct mCore;
void observe_attach(struct mCore *core, const char *directory);
bool observe_command(const char *line);
bool observe_before_store(uint32_t address, unsigned width);
void observe_instruction(uint32_t pc, uint32_t sp, uint32_t lr, unsigned mode, bool entered);
void observe_after_store(uint32_t pc);
void observe_close(void);
#else
#define observe_attach(core, directory) ((void)0)
#define observe_command(line) false
#define observe_before_store(address, width) false
#define observe_instruction(pc, sp, lr, mode, entered) ((void)(entered))
#define observe_after_store(pc) ((void)0)
#define observe_close() ((void)0)
#endif

#endif
