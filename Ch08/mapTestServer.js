// Required modules
var express=require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http); 
app.use(express.static('src'));

var bin = [ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
	    11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
	    21,  22,  23,  24,  25];
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

console.log(binmap);
var binPredicted = 0;

function updateWebsite(){
    binPredicted = (binPredicted+1)%(bin.length);
    console.log(binPredicted);
    var location = binmap[binPredicted];
    io.emit('data',{x:location[0], y:location[1]}); //send data to website
}
setInterval(updateWebsite, 1000);

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

