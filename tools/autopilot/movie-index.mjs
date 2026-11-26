// Count the input reads before frame, including the case where frame itself
// had no read. Harness commands at this index run before the next read.
export function tickAt(readFrames, frame) {
  let lo = 0, hi = readFrames.length;
  while (lo < hi) {
    const mid = (lo + hi) >>> 1;
    if (readFrames[mid] < frame) lo = mid + 1;
    else hi = mid;
  }
  return lo;
}
