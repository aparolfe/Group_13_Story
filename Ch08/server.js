// Required modules
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
console.log(data.length);
console.log(bin.length);

var binmap= {};
bin.forEach(function(entry) { // maps bin to x,y location for website
    var x,y;
    if (entry < 19)
    { x = 7;
      y = entry+1; }
    else if (entry < 22)
    { x = 7 - 1.5*(entry-18);
      y = 19; }
    else if (entry < 41)
    { x = 1;
      y = 41 - entry; }
    else
    { x = 1+ 1.5 * (entry - 40)
      y = 1; }
    binmap[entry] = [x,y];
});

// command line args setup
// command line syntax: node server.js xbeePort arduinoPort webcamPort
var xbeePort = process.argv[2];
var arduinoPort = process.argv[3];
var webcamPort = process.argv[4];

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
var webcamPortConfig = {
    baudRate: 9600
};

var xbeeSerial, arduinoSerial, webcamSerial;
xbeeSerial = new SerialPort.SerialPort(xbeePort, xbeePortConfig);
//arduinoSerial = new SerialPort.SerialPort(arduinoPort, arduinoPortConfig);
//webcamSerial = new SerialPort.SerialPort(webcamPort, webcamPortConfig);

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
	//console.log(rssi_total);
    }
});
/*
arduinoSerial.on("open", function () {
    console.log('arduino serial open');
});

/*webcamSerial.on("open", function () {
    console.log('webcam serial open');
}); 
*/

// Knn Logic
var bin_predicted = 0;

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

function predict_and_send(){
    console.log(rssi_total);
    if (rssi_total.indexOf(0) == -1 && rssi_total.indexOf(255) == -1) { // if we have good data from all beacons
	bin_predicted= predict(rssi_total, knn);	//predict bin, then clear values
	xbee_11_rssi = 0;
	xbee_12_rssi = 0;
	xbee_13_rssi = 0;
	xbee_14_rssi = 0;
	console.log(bin_predicted);
	if (bin_predicted in binmap) {	// only update website if prediction is valid
	    var location = binmap[bin_predicted];
	    console.log(location);
	    io.emit('data',{x:location[0], y:location[1]});
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

