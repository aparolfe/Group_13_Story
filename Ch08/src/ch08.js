$(window).keypress(function(event) {
    var keypressed = String.fromCharCode(event.keyCode);
    socket.emit('update',keypressed);
    console.log(keypressed);
    switch (event.keyCode) {
    case 97: //KEY: A
	    left();
        manual();
       	break;
    case 119: //KEY: W
        up();
        manual();
        break;
    case 100: //KEY: D
        right();
        manual();
        break;
    case 122: //KEY: Z
        down();
        manual();
        break;
    case 13: //ENTER
        $('#control').click();
        break;
    case 32: //SPACE
        $('#mode').click();
        break;
    case 49: //The number 1
        auto();
        break;
    case 48: //the number 0
        stop();
        break;
    }
});

function up(){
	$('#up').css("color","green");	
	$('#down').css("color","black");
	$('#left').css("color","black");
	$('#right').css("color","black");
}

function down(){
        $('#up').css("color","black");           
        $('#down').css("color","green");
        $('#left').css("color","black");
        $('#right').css("color","black");
}

function left(){
        $('#up').css("color","black");           
        $('#down').css("color","black");
        $('#left').css("color","green");
        $('#right').css("color","black");
}

function right(){
        $('#up').css("color","black");           
        $('#down').css("color","black");
        $('#left').css("color","black");
        $('#right').css("color","green");

function start(){
    $('#control').val("START");
    $('#control').css("background-color","green");
    socket.emit('control',1); }

function stop(){
    $('#control').val("STOP");
    $('#control').css("background-color","red");
    $('#up').css("color","black");  
    $('#down').css("color","black");
    $('#left').css("color","black");
    $('#right').css("color","black");
    socket.emit('control',0);  }

function auto(){
    $('#mode').val("AUTO");
    $('#mode').css("background-color","purple");
    socket.emit('control',1); }

function manual(){
    $('#mode').val("MANUAL");
    $('#mode').css("background-color","blue");
//    socket.emit('mode',0); }

$('.bton').click(function(){
    var value = $(this).val();
    if(value == "START"){
        stop();}
    else if(value == "STOP"){
        start();}
    else if(value == "MANUAL"){
        auto();}
//    else if(value == "AUTO"){
//        manual();}	
});
