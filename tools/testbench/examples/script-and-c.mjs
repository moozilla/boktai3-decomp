import { scenario } from '../scenario.mjs';

export default scenario({ stage: 'warp', scene: 2744, entrance: 57, story: 14 }, function* (s) {
  s.expect(s.where().scene).equals(2744);
  s.setVariable('r700:0x3fa:t1', 42);
  s.expect(s.variable('r700:0x3fa:t1')).equals(42);
  // 1206 is the shared no-op called at the end of enemy library procedures.
  s.callproc(1206);
  s.expect(s.call('Div', 63, 7)).equals(9);
  const snippet = s.c('RESULT(Div(63, 7));');
  s.expect(snippet.result).equals(9);
  yield s.run(12);
  return snippet.result;
});
