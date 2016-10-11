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
 /*   setTimeout(function(){
	sp.write('5,1.');
	sp.write('6,1.');
	sp.write('7,1.');
	sp.write('8,1.');
	sp.write('13,1.');
    },2000); */
    setInterval(function(){
	if (led == 0) {
	    sp.write('5,0.');
	    sp.write('6,0.');
	    sp.write('7,0.');
      sp.write('8,0.');
	    sp.write('13,0.');
	    led = 1;
	}
	else {
    sp.write('5,1.');
    sp.write('6,1.');
    sp.write('7,1.');
    sp.write('8,1.');
    sp.write('13,1.');
	    led = 0;
	}
	}, 2000);
  sp.on('data', function(data) {
      // sanitize input data
      if (data.length==7) {
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
    var cmd = req.something;
    //send to serial port
    sp.write(data, function () {
	sp.drain(console.log('sent command: ' + cmd));
  });
    //don't send anything back to website
});
