var SerialPort = require("serialport");
var express=require('express');
var app = express();
var xbee_api = require('xbee-api');
var ml = require('machine_learning');
var xbee_11_rssi = 0;
var xbee_12_rssi = 0;
var xbee_13_rssi = 0;
var xbee_14_rssi = 0;
var rssi_total =[];
var http = require('http').Server(app);
var io = require('socket.io')(http); 
app.use(express.static('src'));

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 2000;


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
var portConfig = {
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


// y raw data
var y_result_corridor =
[ 0,  0,
  1,  1,  1,
  2,  2,  2,
  3,  3,  3,
  4,  4,  4,
  5,  5,  5,
  6,  6,  6,
  7,  7,  7,
  8,  8,  8,
  9,  9,  9,
  10,  10,  10,
  11,  11,  11,
  12,  12,  12,
  13,  13,  13,
  14,  14,  14,
  15,  15,  15,
  16,  16,  16,
  17,  17,  17,
  18,  18,  18,
  19,  19,  19,
  20,  20,  20 ]

var y_data_corridor_1 =
[ [ 66, 67, 72, 75 ],  [ 77, 75, 58, 76 ],
  [ 75, 81, 52, 73 ],  [ 62, 71, 52, 73 ],  [ 59, 75, 49, 77 ],
  [ 64, 73, 52, 76 ],  [ 61, 73, 53, 81 ],  [ 69, 75, 57, 78 ],
  [ 70, 75, 57, 77 ],  [ 59, 69, 67, 71 ],  [ 72, 74, 67, 87 ],
  [ 63, 67, 70, 80 ],  [ 60, 81, 58, 81 ],  [ 62, 74, 71, 82 ],
  [ 56, 70, 69, 83 ],  [ 58, 68, 70, 88 ],  [ 67, 66, 59, 88 ],
  [ 70, 75, 68, 89 ],  [ 57, 64, 72, 85 ],  [ 59, 63, 77, 97 ],
  [ 73, 69, 72, 95 ],  [ 52, 63, 67, 86 ],  [ 60, 76, 67, 84 ],
  [ 61, 64, 65, 89 ],  [ 66, 71, 71, 89 ],  [ 60, 65, 70, 84 ],
  [ 63, 70, 76, 86 ],  [ 55, 75, 70, 86 ],  [ 59, 73, 72, 96 ],
  [ 61, 76, 78, 84 ],  [ 79, 68, 73, 87 ],  [ 69, 63, 71, 93 ],
  [ 56, 65, 81, 87 ],  [ 70, 66, 82, 97 ],  [ 60, 68, 69, 94 ],
  [ 60, 62, 75, 97 ],  [ 67, 61, 78, 96 ],  [ 72, 68, 80, 93 ],
  [ 55, 57, 79, 98 ],  [ 60, 65, 82, 97 ],  [ 49, 56, 78, 96 ],
  [ 59, 58, 75, 92 ],  [ 54, 74, 84, 96 ],  [ 57, 54, 80, 97 ],
  [ 52, 58, 86, 95 ],  [ 49, 59, 78, 95 ],  [ 63, 62, 81, 95 ],
  [ 60, 54, 84, 96 ],  [ 51, 47, 79, 95 ],  [ 54, 51, 77, 95 ],
  [ 56, 54, 84, 96 ],  [ 49, 51, 74, 96 ],  [ 47, 57, 82, 96 ],
  [ 44, 58, 79, 98 ],  [ 51, 53, 83, 98 ],  [ 45, 51, 84, 97 ],
  [ 48, 49, 77, 97 ],  [ 45, 41, 80, 97 ],  [ 35, 42, 73, 97 ],
  [ 48, 41, 75, 98 ],  [ 38, 44, 83, 98 ],  [ 34, 45, 78, 98 ] ]

var y_data_corridor_2 =
[ [ 71, 52, 39, 79 ],  [ 77, 55, 37, 84 ],
  [ 77, 57, 37, 80 ],  [ 76, 69, 46, 88 ],  [ 80, 63, 46, 81 ],
  [ 67, 50, 47, 79 ],  [ 74, 51, 50, 81 ],  [ 72, 53, 44, 86 ],
  [ 85, 57, 51, 86 ],  [ 80, 61, 51, 87 ],  [ 75, 62, 48, 98 ],
  [ 67, 61, 56, 85 ],  [ 70, 64, 45, 95 ],  [ 74, 61, 55, 85 ],
  [ 69, 65, 60, 96 ],  [ 69, 54, 55, 97 ],  [ 68, 62, 55, 90 ],
  [ 78, 74, 49, 96 ],  [ 74, 55, 54, 94 ],  [ 69, 71, 63, 94 ],
  [ 78, 55, 69, 92 ],  [ 68, 64, 72, 97 ],  [ 78, 63, 57, 92 ],
  [ 69, 52, 60, 96 ],  [ 72, 58, 54, 97 ],  [ 66, 51, 60, 96 ],
  [ 73, 49, 63, 98 ],  [ 67, 46, 62, 98 ],  [ 65, 46, 65, 97 ],
  [ 75, 47, 69, 97 ],  [ 66, 62, 66, 97 ],  [ 70, 49, 59, 97 ],
  [ 70, 50, 60, 98 ],  [ 61, 54, 60, 98 ],  [ 60, 53, 75, 98 ],
  [ 67, 49, 66, 98 ],  [ 71, 59, 75, 98 ],  [ 64, 45, 71, 98 ],
  [ 62, 52, 76, 98 ],  [ 79, 46, 65, 98 ],  [ 63, 49, 63, 98 ],
  [ 65, 61, 76, 98 ],  [ 72, 54, 70, 98 ],  [ 59, 49, 80, 98 ],
  [ 58, 46, 69, 98 ],  [ 57, 55, 67, 98 ],  [ 55, 56, 71, 98 ],
  [ 64, 48, 72, 98 ],  [ 53, 51, 69, 98 ],  [ 58, 55, 69, 98 ],
  [ 57, 44, 69, 98 ],  [ 51, 38, 82, 98 ],  [ 60, 50, 73, 98 ],
  [ 58, 47, 75, 98 ],  [ 53, 43, 86, 98 ],  [ 51, 41, 76, 98 ],
  [ 52, 36, 77, 98 ],  [ 52, 37, 76, 98 ],  [ 48, 41, 79, 98 ],
  [ 48, 37, 75, 98 ],  [ 48, 33, 73, 98 ],  [ 49, 33, 76, 98 ] ]

var y_data_short_ends=
[ [ 75, 79, 58, 66 ],  [ 72, 80, 58, 69 ],  [ 57, 77, 57, 73 ],  [ 70, 79, 64, 76 ],  [ 81, 82, 49, 74 ],  [ 82, 69, 41, 85 ],  [ 84, 73, 49, 79 ],  [ 80, 63, 61, 81 ],  [ 73, 55, 36, 84 ],
  [ 84, 72, 59, 75 ],  [ 74, 54, 44, 81 ],  [ 82, 56, 59, 74 ],  [ 82, 65, 50, 76 ],  [ 81, 71, 48, 80 ],  [ 84, 69, 43, 81 ],  [ 78, 79, 48, 77 ],  [ 78, 79, 48, 88 ],
  [ 67, 77, 51, 81 ],  [ 63, 72, 58, 72 ],  [ 69, 71, 55, 72 ],  [ 74, 74, 55, 77 ],  [ 79, 75, 53, 73 ],  [ 83, 78, 47, 70 ],  [ 78, 69, 54, 72 ],  [ 81, 70, 51, 79 ],  [ 82, 72, 52, 69 ],  [ 81, 79, 46, 81 ],
  [ 79, 72, 42, 78 ],  [ 59, 44, 74, 95 ],  [ 45, 51, 75, 95 ],  [ 53, 45, 78, 95 ],  [ 38, 36, 81, 95 ],  [ 37, 37, 79, 95 ],  [ 37, 51, 83, 95 ],  [ 43, 48, 86, 98 ],  [ 40, 61, 83, 98 ],  [ 49, 41, 78, 93 ],
  [ 38, 50, 86, 96 ],  [ 41, 45, 86, 94 ],  [ 45, 36, 83, 98 ],  [ 43, 40, 82, 98 ],  [ 39, 38, 81, 98 ],  [ 49, 42, 86, 98 ],  [ 50, 41, 83, 98 ],  [ 51, 49, 81, 98 ],  [ 51, 36, 82, 98 ],  [ 41, 50, 78, 98 ],
  [ 46, 36, 74, 98 ],  [ 49, 37, 67, 98 ],  [ 51, 37, 69, 98 ],  [ 41, 44, 82, 98 ],  [ 43, 39, 79, 98 ],  [ 43, 43, 84, 98 ],  [ 46, 50, 81, 98 ],  [ 45, 47, 87, 98 ],  [ 43, 61, 83, 98 ],  [ 36, 57, 84, 96 ] ]

var y_result_short_ends =
[ 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 18.5, 18.5, 18.5, 18.5, 18.5, 18.5, 18.5, 18.5, 18.5, 18.5,
 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 19.5, 19.5, 19.5, 19.5, 19.5, 19.5, 19.5, 19.5, 19.5, 19.5 ]

// x raw data
var x_data_elevator_end=
[ [ 74, 73, 55, 75 ],  [ 74, 73, 55, 73 ],
  [ 75, 74, 61, 73 ],  [ 73, 79, 53, 68 ],  [ 73, 83, 51, 69 ],
  [ 72, 83, 44, 74 ],  [ 80, 84, 50, 84 ],  [ 86, 84, 51, 72 ],
  [ 75, 74, 48, 69 ],  [ 75, 78, 45, 74 ],  [ 77, 85, 48, 69 ],
  [ 76, 77, 55, 79 ],  [ 84, 80, 45, 92 ],  [ 84, 74, 53, 92 ],
  [ 80, 86, 49, 74 ],  [ 83, 83, 43, 91 ],  [ 80, 82, 40, 87 ],
  [ 79, 75, 45, 89 ],  [ 80, 70, 36, 80 ],  [ 82, 57, 49, 78 ],
  [ 74, 63, 43, 83 ],  [ 71, 59, 47, 83 ] ]

var x_result_elevator_end=
[ -0.5, -0.5,
    1.5,  1.5,  1.5,
    2.5,  2.5,  2.5,
    3.5,  3.5,  3.5,
    4.5,  4.5,  4.5,
    5.5,  5.5,  5.5,
    6.5,  6.5,  6.5,
    7.5,  7.5 ]

var x_data_window_end =
[ [ 39, 43, 76, 89 ],  [ 38, 54, 78, 90 ],
  [ 42, 58, 87, 92 ],  [ 45, 45, 77, 94 ],  [ 43, 43, 78, 97 ],
  [ 45, 47, 85, 97 ],  [ 58, 41, 79, 97 ],  [ 51, 43, 83, 97 ],
  [ 46, 49, 87, 97 ],  [ 58, 44, 85, 97 ],  [ 51, 51, 80, 97 ],
  [ 48, 49, 78, 97 ],  [ 45, 41, 83, 97 ],  [ 43, 51, 81, 97 ],
  [ 50, 43, 81, 97 ],  [ 51, 38, 80, 97 ],  [ 51, 35, 77, 97 ],
  [ 46, 42, 81, 97 ],  [ 44, 39, 75, 97 ],  [ 52, 51, 73, 97 ],
  [ 49, 42, 71, 97 ],  [ 60, 38, 71, 97 ],  [ 44, 35, 75, 97 ] ]
  
var x_result_window_end=
[ -0.5, -0.5,
    1.5,  1.5,  1.5,
    2.5,  2.5,  2.5,
    3.5,  3.5,  3.5,
    4.5,  4.5,  4.5,
    5.5,  5.5,  5.5,
    6.5,  6.5,  6.5,
    7.5,  7.5,  7.5 ]

var x_data_corridor_1=[[ 64, 79, 59, 75 ],
  [ 69, 75, 77, 75 ],
  [ 51, 77, 51, 80 ],
  [ 58, 72, 64, 80 ],
  [ 62, 84, 61, 76 ],
  [ 60, 78, 52, 74 ],
  [ 50, 84, 57, 75 ],
  [ 50, 70, 59, 83 ],
  [ 58, 76, 67, 82 ],
  [ 58, 82, 75, 92 ],
  [ 53, 77, 67, 86 ],
  [ 55, 69, 70, 90 ],
  [ 54, 66, 67, 92 ],
  [ 51, 66, 61, 98 ],
  [ 54, 74, 66, 88 ],
  [ 61, 66, 72, 89 ],
  [ 45, 76, 80, 93 ],
  [ 59, 63, 79, 94 ],
  [ 50, 61, 76, 96 ],
  [ 72, 66, 76, 97 ],
  [ 60, 70, 78, 92 ],
  [ 53, 69, 69, 92 ],
  [ 52, 73, 78, 95 ],
  [ 50, 70, 73, 92 ],
  [ 48, 65, 75, 92 ],
  [ 53, 60, 79, 96 ],
  [ 45, 62, 75, 94 ],
  [ 46, 68, 86, 95 ],
  [ 49, 57, 83, 96 ],
  [ 44, 60, 77, 90 ],
  [ 38, 54, 83, 96 ]]

var x_result_corridor_1 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

var x_data_corridor_2 = [ [ 81, 72, 48, 89 ],
  [ 81, 72, 54, 93 ],
  [ 71, 66, 47, 86 ],
  [ 70, 62, 51, 90 ],
  [ 77, 63, 53, 91 ],
  [ 76, 66, 48, 88 ],
  [ 69, 63, 63, 87 ],
  [ 75, 59, 64, 93 ],
  [ 71, 57, 69, 89 ],
  [ 74, 63, 52, 97 ],
  [ 72, 72, 59, 94 ],
  [ 70, 55, 58, 96 ],
  [ 75, 62, 60, 96 ],
  [ 75, 65, 62, 97 ],
  [ 69, 66, 69, 98 ],
  [ 72, 59, 62, 98 ],
  [ 63, 55, 66, 98 ],
  [ 75, 60, 71, 95 ],
  [ 66, 57, 65, 95 ],
  [ 68, 56, 60, 95 ],
  [ 65, 55, 62, 95 ],
  [ 67, 57, 74, 95 ],
  [ 59, 55, 69, 95 ],
  [ 67, 59, 73, 95 ],
  [ 61, 52, 69, 95 ],
  [ 62, 50, 76, 95 ],
  [ 64, 55, 69, 95 ],
  [ 68, 47, 69, 95 ],
  [ 54, 57, 70, 95 ],
  [ 56, 46, 72, 95 ],
  [ 58, 44, 62, 97 ],
  [ 55, 45, 69, 97 ],
  [ 51, 55, 67, 97 ] ]

var x_result_corridor_2 =[7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7]

// Final Training data
var y_data = y_data_corridor_1 .concat(y_data_corridor_2).concat(y_data_short_ends).concat(x_data_corridor_2)
var y_result = y_result_corridor.concat(y_result_corridor).concat(y_result_short_ends).concat(x_result_corridor_2)
var x_data = x_data_elevator_end.concat(x_data_window_end).concat(x_data_corridor_1)
var x_result = x_result_elevator_end.concat(x_result_window_end).concat(x_result_corridor_1)

function predict(input,knn){
        var predicted = knn.predict({
                x: input,
                k: 3,
                weightf : {type : "none"},//'gaussian', sigma : 10.0},
                distance : {type : 'euclidean'}
        });
        return predicted
}

var x_knn = new ml.KNN({
    data : x_data,
    result : x_result
});

var y_knn = new ml.KNN({
    data : y_data,
    result : y_result
});

function send(x_predicted,y_predicted){
        io.emit('data',{x:x_predicted,y:y_predicted});
}

var x_predicted;
var y_predicted;

function predict_and_send(){
	console.log(rssi_total);
	x_predicted= predict(rssi_total,x_knn);
	y_predicted = predict(rssi_total,y_knn);
	console.log(x_predicted,y_predicted);
	io.emit('data',{x:x_predicted,y:y_predicted});        
}


//var x_predicted = x_knn.predict({
//        x: x_input,
//        k: 3,
//        weightf : {type : 'gaussian', sigma : 10.0},
//        distance : {type : 'euclidean'}
//});
//
//
//
//
//var y_predicted = y_knn.predict({
//        x: y_input,
//        k: 3,
//        weightf : {type : 'gaussian', sigma : 10.0},
//        distance : {type : 'euclidean'}
//});


io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('update',function(msg){
        console.log(msg);
        sp.write(msg);
        });
});


setInterval(predict_and_send,2000);

http.listen(3000, function(){
  console.log('listening on *:3000');
});


// Webpage with LED status
app.get('/', function(req, res){
    res.sendFile('index.html' , { root : __dirname});
});

