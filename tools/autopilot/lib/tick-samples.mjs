// KEYINPUT stores the ten controller bits active-low in little-endian I/O.
export function readPressedKeys(memory, offset = 0x04000130) {
  return ~memory.getUint16(offset, true) & 0x03ff;
}

// Align snapshots at input polls and report the earliest differing sample.
export function compareSamples(rom, port, fields, frames, reads, expectedReads) {
  const size = fields.reduce((n, field) => n + field.size, 0);
  for (let tick = 0; tick < port.length; tick++) {
    const record = rom.subarray(tick * size, (tick + 1) * size);
    const inputDiffers = reads[tick] !== expectedReads[tick];
    const complete = record.length === size;
    if (!complete && !inputDiffers) break;
    if (record.equals(port[tick]) && !inputDiffers) continue;
    const changes = {};
    if (inputDiffers) changes.input = { port: reads[tick], rom: expectedReads[tick] ?? null };
    if (complete) {
      let offset = 0;
      for (const { name, size } of fields) {
        const a = port[tick].subarray(offset, offset + size);
        const b = record.subarray(offset, offset + size);
        if (!a.equals(b)) changes[name] = { port: a.toString('hex'), rom: b.toString('hex') };
        offset += size;
      }
    }
    return { tick, frame: frames[tick], changes };
  }
  if (rom.length !== port.length * size) return { portTicks: port.length, romTicks: rom.length / size };
  if (!port.length) return { error: 'No input ticks sampled' };
  return null;
}
