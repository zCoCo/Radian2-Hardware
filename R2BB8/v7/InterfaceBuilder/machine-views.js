'use strict';

const path = require('path');
const fs = require('fs');

const gen_util = require('gen_util');
const machine_render = require('machine_render');

$(() => {
  // Set Up Machine Configuration:
  let machine_id = 's160';
  let machine_config = JSON.parse(fs.readFileSync(path.join(__dirname, 'machine_config', machine_id + '.json'), 'utf8'));

  // Get Drawing Elements:
  const canvas_top = $('.machine-topView')[0]; // Top View
  const ctx_top = canvas_top.getContext('2d');

  // Invoke Renderer:
  const top_view = new machine_render.Drawing(
    machine_id,
    machine_config.numActivePods,
    ctx_top,
    canvas_top.width,
    canvas_top.height
  );

  // Set Sizing:
  gen_util.holdAspectRatio($('.machine-topView'), 1, function(){
                                                                                console.log('Refreshing Top View. . .')
    top_view.refresh();
  });

});
