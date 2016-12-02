//var socket = io();

$(window).keypress(function(event) {
    //socket.emit('keypress',event.keyCode)
    console.log(event.keyCode);
    switch (event.keyCode) {
    case 97:
	left();
    	console.log(event.keyCode);
       	break;
    case 119:
        up();
    	console.log(event.keyCode);
        break;
    case 100:
        right();
    	console.log(event.keyCode);
        break;
    case 115:
        down();
    	console.log(event.keyCode);
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
}
