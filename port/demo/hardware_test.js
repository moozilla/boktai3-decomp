// Run the browser's actual wasm C audio renderer in Node, without Web Audio.
'use strict';
const fs=require('fs');
(async()=>{
  const {instance:{exports:e}}=await WebAssembly.instantiate(fs.readFileSync('port/demo/hardware.wasm'),{});
  const blocks=[];let peak=0,energy=0;
  if(process.argv[2]){
    const rom=fs.readFileSync(process.argv[2]);if(rom.length>e.demo_rom_capacity())throw Error('ROM too large');
    new Uint8Array(e.memory.buffer,e.demo_rom_buffer(),rom.length).set(rom);e.demo_load_rom(rom.length);
    if(e.demo_start(1)<0)throw Error('Song 1 failed to load');
  }else e.demo_psg(0,1750);
  for(let frame=0;frame<597;frame++){
    const n=e.demo_render();if(n!==352||e.demo_error())throw Error('Render failed at frame '+frame);
    const pcm=new Int16Array(new Int16Array(e.memory.buffer,e.demo_pcm_buffer(),n*2));blocks.push(pcm);
    for(const sample of pcm){peak=Math.max(peak,Math.abs(sample));energy+=sample*sample}
  }
  if(!energy)throw Error('Output was silent');
  const count=blocks.reduce((n,b)=>n+b.length,0),wav=Buffer.alloc(44+count*2);
  wav.write('RIFF');wav.writeUInt32LE(36+count*2,4);wav.write('WAVEfmt ',8);wav.writeUInt32LE(16,16);wav.writeUInt16LE(1,20);wav.writeUInt16LE(2,22);wav.writeUInt32LE(21024,24);wav.writeUInt32LE(84096,28);wav.writeUInt16LE(4,32);wav.writeUInt16LE(16,34);wav.write('data',36);wav.writeUInt32LE(count*2,40);
  let offset=44;for(const b of blocks)for(const sample of b){wav.writeInt16LE(sample,offset);offset+=2}
  fs.mkdirSync('build/demo',{recursive:true});const name=process.argv[2]?'hardware-song-1':'hardware-pulse';fs.writeFileSync('build/demo/'+name+'.wav',wav);
  fs.writeFileSync('build/demo/'+name+'.json',JSON.stringify({rate:21024,frames:count/2,peak,rms:Math.sqrt(energy/count),notes:e.demo_notes(),skipped:e.demo_skipped(),waveform:Array.from(blocks[40])}));
  console.log(`Browser C/wasm: PASS; ${e.demo_notes()} notes, ${e.demo_skipped()} skips, peak ${peak}; build/demo/${name}.wav`);
})().catch(error=>{console.error(error);process.exitCode=1});
