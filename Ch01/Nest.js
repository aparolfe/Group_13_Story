var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var average = function(inputArray ){
    var tempDict = {};
    for (var ii = 0; ii < inputArray.length; ii++) {
	var data = inputArray[ii];
	var value = data.substring(data.length-6, data.length)
	var id = data.substring(0, data.length-6)
	tempDict[id] = value
    };
    var sum = 0;
    Object.keys(tempDict).forEach(function(key) {
	sum = sum + parseFloat(tempDict[key]);
    });
    return sum/(Object.keys(tempDict).length);
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
    console.log('open');
    var dataArray = [];
  sp.on('data', function(data) {
      console.log('data received: ' + data);
      dataArray.push(data);
      console.log(average(dataArray));
      var displayTemp = average(dataArray);
      io.emit("chat message", "Average Temperature: " + displayTemp);
  });
});

