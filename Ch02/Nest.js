var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('test.db');
var math = require('mathjs');

var dataArray = [];
var updateDisplayFlag = false;
var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

function getDateTime() {
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

function tempAverage(inputArray ) {
    // Calculate average, high, low
    if (dataArray.length > 0) {
	var tempDict = {};
	for (var ii = 0; ii < inputArray.length; ii++) {
	    var data = inputArray[ii];
	    var value = data.substring(data.length-6, data.length)
	    var id = data.substring(0, data.length-6)
	    tempDict[id] = value;
	    inputArray[ii] = value;
	};
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
	if (updateDisplayFlag) {
	    io.emit("data", {time:getDateTime(), high:max, current:avg, low:min});
	}
	//insert into db    
	try{ 
	    db.serialize(function(){		
	    	db.run("CREATE TABLE IF NOT EXISTS temp (datetime TEXT, avgtemp REAL, hightemp REAL, lowtemp REAL)");
		console.log("insert into db")
		var stmt = db.prepare("INSERT INTO temp VALUES(?,?,?,?)");
		stmt.run(Date(), avg, max, min);
		stmt.finalize();
		});
	} catch(e) {
	    console.log("An error has occurred",e);
	}
	dataArray=[];
    }
};	

io.on('connection', function(socket){
  console.log('a user connected');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);
sp.on("open", function () {
    console.log('open');
    setInterval(function(){tempAverage(dataArray);}, 2000);
  sp.on('data', function(data) {
      console.log('data received: ' + data);
      // sanitize input data
      if (data.length==7 && data.substring(data.length-4,data.length-3) == ".") {
	  dataArray.push(data);
      }
  });
});

// default page with current temperature and session graph
app.get('/', function(req, res){
    updateDisplayFlag = true;
    res.sendFile('index.html' , { root : __dirname});
});

// historical view page
app.get('/history', function(req,res){
    updateDisplayFlag = false;
    res.sendFile('history.html' , { root : __dirname});
    setTimeout(function(){
	var datetime = [];
        var avgtemp = [];
        var hightemp = [];
        var lowtemp = [];
	db.all("SELECT * FROM temp", function(err,rows){
	    rows.forEach(function(row){
		datetime.push(row.datetime.substring(0,row.datetime.length-15));
		avgtemp.push(row.avgtemp);	
		hightemp.push(row.hightemp);
		lowtemp.push(row.lowtemp);
	    });
	    io.emit("data", {time:datetime, high:hightemp, current:avgtemp, low:lowtemp});
	    console.log("sent db data");
	});
    },500); 
});



