'use strict';
const fs = require('fs');
(async () => {
  const {instance} = await WebAssembly.instantiate(fs.readFileSync(process.argv[2]), {});
  const line = instance.exports.hw_test_run();
  if (line) throw new Error(`Hardware test failed at hw_test.c:${line}`);
  console.log('Hardware tests: PASS (wasm32)');
})().catch(error => { console.error(error); process.exitCode = 1; });
