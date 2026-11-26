import { scenario } from '../scenario.mjs';

export default scenario({ empty: true, time: '14:00', flags: { 'r708:0x3ed.2:t4': 1 } }, function* (s) {
  s.expect(s.where().scene).equals(2776);
  s.expect(s.variable('r708:0x3ed.2:t4')).equals(1);
  yield s.press('right', 12);
  yield s.run(30);
  return s.where();
});
