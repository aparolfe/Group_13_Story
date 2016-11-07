var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var xbee_11_rssi = 0;
var xbee_12_rssi = 0;
var xbee_13_rssi = 0;
var xbee_14_rssi = 0;
var rssi_total =[];

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 3000;


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
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


//  console.log("Beacon ID"+frame.type );
  //console.log(frame.type);
  if (frame.type >= 140){

    if (frame.data[1] == 11)
      { xbee_11_rssi = frame.data[0];}
    if (frame.data[1] == 12)
      { xbee_12_rssi = frame.data[0];}
    if (frame.data[1] == 13)
      { xbee_13_rssi = frame.data[0];}
    if (frame.data[1] == 14)
     { xbee_14_rssi = frame.data[0];}
//console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
rssi_total =[xbee_11_rssi,xbee_12_rssi,xbee_13_rssi,xbee_14_rssi];
    console.log(rssi_total);

  }
});
