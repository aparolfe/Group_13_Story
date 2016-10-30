var SerialPort = require("serialport");	//communicate with Xbee coordinator
var express=require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

// uncomment when ready to talk to xBee
var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);
sp.on("open", function () {
    console.log('Serial port open');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('status', function(msg){
    console.log(msg);
    io.emit('status', msg);
    sp.write(msg + "\n");
  });
});


app.use(express.static('src'));

http.listen(3000, function(){
  console.log('listening on *:3000');
});

app.get('/', function(req, res){
    console.log("Get request received");
    res.sendFile('index.html' , { root : __dirname});
});

app.get('/start', function(req,res) {
    console.log("Start command received");
    res.sendStatus(204);
    sp.write("1");
});

app.get('/stop', function(req,res) {
    console.log("Stop command received");
    res.sendStatus(204);
    sp.write("0");
});
