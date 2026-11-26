// SWI 0 stops the current wasm invocation. Re-enter AgbMain with the original
// shadow stack pointer, keeping the cartridge, clock, frame counter and hooks.
export function runGame(ex, saveSize) {
  const stack = ex.__stack_pointer.value;
  let entry = () => ex.port_game_run(saveSize);
  for (;;) {
    try {
      return entry();
    } catch (error) {
      if (!(error instanceof WebAssembly.RuntimeError) || !ex.port_game_reset_pending?.()) throw error;
    }
    ex.__stack_pointer.value = stack;
    entry = ex.port_game_resume;
  }
}
