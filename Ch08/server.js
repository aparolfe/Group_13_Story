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
var data = require('./trainingData.js');
var y_result = require('./xResult.js');
var x_result = require('./yResult.js');

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
var rssi_total =[];
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
arduinoSerial = new SerialPort.SerialPort(arduinoPort, arduinoPortConfig);
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

arduinoSerial.on("open", function () {
    console.log('arduino serial open');
});

/*webcamSerial.on("open", function () {
    console.log('webcam serial open');
}); 
*/

// Knn Logic
var x_predicted = 0;
var y_predicted = 0;

function predict(input,knn){
    var predicted = knn.predict({
        x: input,
        k: 1,
        weightf : {type : "none"},
        distance : {type : 'euclidean'}
    });
    return predicted;
};

var x_knn = new ml.KNN({
    data : data,
    result : x_result
});

var y_knn = new ml.KNN({
    data : data,
    result : y_result
});

function predict_and_send(){
    console.log(rssi_total);
    x_predicted= predict(rssi_total,x_knn);
    y_predicted = predict(rssi_total,y_knn);
    console.log(x_predicted,y_predicted);
    io.emit('data',{x:x_predicted,y:y_predicted});        
}
setInterval(predict_and_send, rssiDelay);

// connect with website, relay input to arduino
io.on('connection', function(socket){
    console.log('a user connected');
    socket.on('update',function(msg){
	console.log(msg);
	if ("01wadz".includes(msg)) {	// input validation
	    arduinoSerial.write(msg);
	}
    });
});

http.listen(3000, function(){
    console.log('listening on *:3000');
});

app.get('/', function(req, res){
    res.sendFile('index.html' , { root : __dirname});
});

