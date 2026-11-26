import assert from 'node:assert/strict';
import test from 'node:test';
import { tickAt } from './movie-index.mjs';

test('map frame marks to the next input tick across gaps and at the end', () => {
  const reads = [1, 2, 4, 8];
  assert.deepEqual(Array.from({ length: 11 }, (_, frame) => tickAt(reads, frame)),
    [0, 0, 1, 2, 2, 3, 3, 3, 3, 4, 4]);
  assert.equal(tickAt([], 10), 0);
});
