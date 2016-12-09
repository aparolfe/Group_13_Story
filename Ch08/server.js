// Required modules
var moment = require('moment');
var SerialPort = require("serialport");
var express=require('express');
var app = express();
var xbee_api = require('xbee-api');
var ml = require('machine_learning');
var http = require('http').Server(app);
var io = require('socket.io')(http); 
app.use(express.static('src'));
// Knn Data load
var data = require('./knnTrainingSet.js');
var bin = require('./binResult.js');
var binmap= {};
bin.forEach(function(entry) { // maps bin to x,y location for website
    var x,y;
    if (entry < 10)
    { x = 7;
      y = entry+1; }
    else if (entry < 14)
    { x = 7 - 1.5*(entry-9);
      y = 10; }
    else if (entry < 23)
    { x = 1;
      y = 23 - entry; }
    else
    { x = 1+ 1.5 * (entry - 22)
      y = 1; }
    binmap[entry] = [x,y];
});

// command line args setup
// command line syntax: node server.js xbeePort arduinoPort webcamPort
var xbeePort = process.argv[2];
var arduinoPort = process.argv[3];

// xbee beacon communication setup
var xbee_11_rssi = 0;
var xbee_12_rssi = 0;
var xbee_13_rssi = 0;
var xbee_14_rssi = 0;
var rssi_total =[xbee_11_rssi,xbee_12_rssi,xbee_13_rssi,xbee_14_rssi];
var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
    api_mode: 2
});
var rssiDelay = 2000;  // time between xbee rssi readings

// set up serial ports 
var xbeePortConfig = {
    baudRate: 9600,
    parser: XBeeAPI.rawParser()
};
var arduinoPortConfig = {
    baudRate: 9600
};

var xbeeSerial, arduinoSerial;
xbeeSerial = new SerialPort.SerialPort(xbeePort, xbeePortConfig);
//arduinoSerial = new SerialPort.SerialPort(arduinoPort, arduinoPortConfig);

var RSSIRequestPacket = {
    type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    destination64: "000000000000ffff",
    broadcastRadius: 0x01,
    options: 0x00,
    data: "test"
}

var requestRSSI = function(){
    xbeeSerial.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

xbeeSerial.on("open", function () {
    console.log('xbee serial open');
    requestRSSI();
    setInterval(requestRSSI, rssiDelay);
});

XBeeAPI.on("frame_object", function(frame) {
    if (frame.type >= 140){
	if (frame.data[1] == 11)
	{ xbee_11_rssi = frame.data[0];}
	if (frame.data[1] == 12)
	{ xbee_12_rssi = frame.data[0];}
	if (frame.data[1] == 13)
	{ xbee_13_rssi = frame.data[0];}
	if (frame.data[1] == 14)
	{ xbee_14_rssi = frame.data[0];}
	rssi_total =[xbee_11_rssi,xbee_12_rssi,xbee_13_rssi,xbee_14_rssi];
    }
});
/*
arduinoSerial.on("open", function () {
    console.log('arduino serial open');
}); */

// Knn Logic

function predict(input,knn){
    var predicted = knn.predict({
        x: input,
        k: 1,
        weightf : {type : "none"},
        distance : {type : 'euclidean'}
    });
    return predicted;
};

var knn = new ml.KNN({
    data : data,
    result : bin
});

var binPredicted = 0;
var lastPredTime = 0;
var lastPrediction = 0;
var crawlerSpeed = 3000; //units ms/bin
function predict_and_send(){
    console.log(rssi_total);
    if (rssi_total.indexOf(0) == -1 && rssi_total.indexOf(255) == -1) { // if we have good data from all beacons
	binPredicted= predict(rssi_total, knn);	//predict bin, then clear values
	xbee_11_rssi = 0;
	xbee_12_rssi = 0;
	xbee_13_rssi = 0;
	xbee_14_rssi = 0;
	console.log(binPredicted);
	if (binPredicted in binmap) {	// if prediction is valid
	    var location = binmap[binPredicted];
//	    console.log(Math.abs(binPredicted-lastPrediction)*crawlerSpeed);
//	    var now = moment();
//	    console.log(moment().diff(lastPrediction));
//	    if (lastPredTime!=0 && ( Math.abs(binPredicted-lastPrediction)*crawlerSpeed < now.diff(lastPredTime) )) { // not very first prediction, teleport check
		io.emit('data',{x:location[0], y:location[1]}); //send data to website
//		console.log("sent data");
//	    }
	    lastPredTime = moment();
	    lastPrediction = binPredicted;
	}
    }
}
setInterval(predict_and_send, rssiDelay);

// connect with website, relay input to arduino
io.on('connection', function(socket){
    console.log('a user connected');
    socket.on('update',function(msg){
	console.log(msg);
	if ("01wadz".includes(msg)) {	// input validation
//	    arduinoSerial.write(msg);
	}
    });
});

http.listen(3000, function(){
    console.log('listening on *:3000');
});

app.get('/', function(req, res){
    res.sendFile('index.html' , { root : __dirname});
});

