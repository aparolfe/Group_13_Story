$(window).keypress(function(event) {
    var keypressed = String.fromCharCode(event.keyCode);
    socket.emit('update',keypressed);
    console.log(keypressed);
    switch (event.keyCode) {
    case 97:
	left();
       	break;
    case 119:
        up();
        break;
    case 100:
        right();
        break;
    case 122:
        down();
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

$('.bton').click(function(){
	
}
