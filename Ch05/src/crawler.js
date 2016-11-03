$('#change').html("Start Crawler");

$('#change').click(function(){
    if ($(this).html() == "Start Crawler") {
	$(this).html("Stop Crawler");
	$(this).css("background-color","crimson");
      	$(this).hover(
	    function(){$(this).css("background","darkred")},
	    function(){$(this).css("background","crimson")}
	);
	$.get( "/start");
	console.log( "Start command sent." );		
    }
    else {
	$(this).html("Start Crawler");
	$(this).css("background-color","forestgreen");
      	$(this).hover(
	    function(){$(this).css("background","darkgreen")},
	    function(){$(this).css("background","forestgreen")}
	);
	$.get( "/stop");
	console.log( "Stop command sent." );
    }
});
