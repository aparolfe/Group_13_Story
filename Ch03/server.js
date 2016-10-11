var SerialPort = require("serialport");	//communicate with Xbee coordinator
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);		// use for real-time update


function getDateTime() {			// use for last sent update
    var curt     = new Date();
    var hour    = curt.getHours();
    var minute  = curt.getMinutes();
    var second  = curt.getSeconds();
    if (hour.toString().length == 1) {
        var hour = '0'+hour;
    }
    if (minute.toString().length == 1) {
        var minute = '0'+minute;
    }
    if (second.toString().length == 1) {
        var second = '0'+second;
    }
    var time = hour+':'+minute+':'+second;
     return time;
}

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;
var led = 0;
sp = new SerialPort.SerialPort(portName, portConfig);
sp.on("open", function () {
    console.log('Serial port open');
  sp.on('data', function(data) {
      // sanitize input data
      if (data.length==7 && data.substring(data.length-2,data.length-1) == ",") {
	  console.log('status: ' + data);
	  // send data on to website
	  io.emit("data", {status:data, time:getDateTime()});
      }
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

io.on('connection', function(socket){
  console.log('a user connected');
});

// Webpage with LED status
app.get('/', function(req, res){
    res.sendFile('index.html' , { root : __dirname});
});
//update
app.get('/update', function(req, res){
    //extract command from req
    var cmd = Object.keys(req.query)[0];
    console.log(cmd);
    //send to serial port
    sp.write(cmd); 
    //don't send anything back to website
});

var special;
app.get('/blink', function(req, res){
    //start randomly changing LED status
    var led = 0;
    special = setInterval(function(){
	if (led == 0) {
	    sp.write('1,0.');
	    sp.write('2,0.');
	    sp.write('3,0.');
	    sp.write('4,0.');
	    sp.write('5,0.');
	    led = 1;
	}
	else {
	    sp.write('1,1.');
	    sp.write('2,1.');
	    sp.write('3,1.');
	    sp.write('4,1.');
	    sp.write('5,1.');
	    led = 0;
	}
}, 2000);
});
app.get('/stop', function(req, res){
    //stop randomly changing LED status
    clearInterval(special);
});
