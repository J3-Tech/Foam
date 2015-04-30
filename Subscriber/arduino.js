var five = require("johnny-five");
var board = new five.Board();

var instance=null;

board.on("ready", function() {
  var anode = new five.Led.RGB({
    pins: {
      red: 3,
      green: 5,
      blue: 6
    },
    isAnode: true
  });

  // Add led to REPL (optional)
  this.repl.inject({
    anode: anode
  });

  // Turn it on and set the initial color
  anode.on();
  anode.color("#000000");

  //anode.blink(1000);
  instance=anode;

});


function test(val){
  var value=parseFloat(val).toFixed(4);
  var multiply = 10000;
  var large=value * multiply;
  var hex = large.toString(16).substring(0,3);
  if(instance){
    instance.color(hex[2]+hex[2]+hex[1]+hex[1]+hex[0]+hex[0]);
  }
}


module.exports=test;