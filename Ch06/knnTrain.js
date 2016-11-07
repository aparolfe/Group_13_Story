// Use to generate knn Training data
// Waits for user to press enter before storing the RSSI values array and position

var SerialPort = require("serialport");
var xbee_api = require('xbee-api');
var xbee_11_rssi = 0;
var xbee_12_rssi = 0;
var xbee_13_rssi = 0;
var xbee_14_rssi = 0;

var data = [];
var result = [];
var max = 8;	// Set for number of positions to train
var n= 3; 	// Set number of data points per position
var ii = 0;
var jj = 0;

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
    api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 3000;

portConfig = {
    baudRate: 9600,
    parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

var RSSIRequestPacket = {
    type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    destination64: "000000000000ffff",
    broadcastRadius: 0x01,
    options: 0x00,
    data: "test"
}

var requestRSSI = function(){
    sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
    console.log('open');
    requestRSSI();
    setInterval(requestRSSI, sampleDelay);
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
    }
});

function waitForUser(callback) {
    'use strict';
    process.stdin.resume();
    process.stdout.write(xbee_11_rssi.toString());
    process.stdout.write(xbee_12_rssi.toString());
    process.stdout.write(xbee_13_rssi.toString());
    process.stdout.write(xbee_14_rssi.toString());
    process.stdout.write(" About to collect RSSI value for position ");
    process.stdout.write(ii.toString());
    process.stdout.write(" : Press enter to continue");
    process.stdin.once("data", function (data) {
        callback();
    });
}

function getDataWaitForUser(max, someCallbackFunction) {
    'use strict';
    function continueProcessing() {
        if (ii < max) {
	    if (jj < n) {
		getData(ii);
		jj++;
	    }
	    else {
		ii++;
		jj = 0;
		continueProcessing();
	    }
        } else {
            someCallbackFunction();
        }
    }
    function getData(position) {
	var datapoint =[xbee_11_rssi,xbee_12_rssi,xbee_13_rssi,xbee_14_rssi];	
	data.push(datapoint);
	result.push(position);
	waitForUser(function () {
            process.stdin.pause();
            continueProcessing();
        });   
    }
    continueProcessing();
}

getDataWaitForUser(max, function () {
    console.log("data");
    console.log(data);
    console.log("result");
    console.log(result);
});
