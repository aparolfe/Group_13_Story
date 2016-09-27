var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('test.db');
var math = require('mathjs');

var dataArray = [];
var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

function getDateTime() {
    var curt     = new Date();
    var year    = curt.getFullYear();
    var month   = curt.getMonth()+1;
    var day     = curt.getDate();
    var hour    = curt.getHours();
    var minute  = curt.getMinutes();
    var second  = curt.getSeconds();
    if(month.toString().length == 1) {
        var month = '0'+month;
    }
    if(day.toString().length == 1) {
        var day = '0'+day;
    }
    if(hour.toString().length == 1) {
        var hour = '0'+hour;
    }
    if(minute.toString().length == 1) {
        var minute = '0'+minute;
    }
    if(second.toString().length == 1) {
        var second = '0'+second;
    }
    var time = hour+':'+minute+':'+second;
     return time;
}

function printAverage(inputArray ) {
    // Calculate average, high, low
    var tempDict = {};
    for (var ii = 0; ii < inputArray.length; ii++) {
	var data = inputArray[ii];
	var value = data.substring(data.length-6, data.length)
	var id = data.substring(0, data.length-6)
	tempDict[id] = value;
	inputArray[ii] = value;
    };
    console.log(tempDict)
    var max = 0;
    var min = 100;
    var sum = 0;
    Object.keys(tempDict).forEach(function(key) {
	var temp = parseFloat(tempDict[key]);
	sum = sum + temp;
	if (temp>max) {max = temp};
	if (temp<min) {min = temp};
    });    
    var avg = sum/(Object.keys(tempDict).length);
    
    // send data to update display
    //io.emit("chat message",{message: "Average Temperature at " + getDateTime() +":" + avg.toFixed(2) + "\xB0 C", temp: tempDict});
    io.emit("data", {high:max, current:avg, low:min});
    //insert into db    
    try{ 
	    console.log(avg,max,min);
	    db.serialize(function(){
	
	    	db.run("CREATE TABLE IF NOT EXISTS temp (datetime TEXT, avgtemp REAL, hightemp REAL, lowtemp REAL)");
		var stmt = db.prepare("INSERT INTO temp VALUES(?,?,?,?)");
		stmt.run(getDateTime(), avg, max, min);
		stmt.finalize();
		});
    }catch(e){
	console.log("An error has occurred",e);
    }
    dataArray=[];
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});


app.get('/query', function(req,res){
	db.all("SELECT * FROM temp", function(err,row){
		res.json(row);
	});
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
    setInterval(function(){printAverage(dataArray);}, 2000);
  sp.on('data', function(data) {
      console.log('data received: ' + data);
      dataArray.push(data);
  });
});
