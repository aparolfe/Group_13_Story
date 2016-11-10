google.charts.load('current', {'packages':['corechart']});
google.charts.setOnLoadCallback(drawChart);

var data;
var chart;
var options = {
    hAxis: {
	minValue: -0.5,
	maxValue: 7.5,
	textPosition: 'none',
	gridlines: {count: 0}
    },
    vAxis: {
	minValue: 0,
	maxValue: 20,
	textPosition: 'none',
	baselineColor: 'white',
	gridlines: {count:0}
    },
    legend: 'none',
    backgroundColor: 'transparent',
    chartArea: {width: '100%', height: '100%'}
};;

var pointCount =10; //number of points shown on the screen
var colors = ['#f3e5f5', '#e1bee7','#ce93d8','#ba68c8','#ab47bc','#9c27b0','#8e24aa','#7b1fa2','#6a1b9a','#4a148c']; // light to dark
var sizes = [1,2,3,4,5,6,7,8,9,10]; // small to big

// given a new point, updates all 10 points and replots chart
function updateChart(x,y) {
    //generate x and y (eventually will be real data)
//    var x = (Math.floor(Math.random() * 6));
//    var y = (Math.floor(Math.random() * 20));
    // add new point, latest and hence boldest
    var style = 'point { size:'+ sizes[pointCount] +'; fill-color:'+ colors[pointCount] +'; }' ;
    var point=[ x , y , style];
    data.addRow([ x, y, style ]);
    var numPoints = data.getNumberOfRows();
    // if too many points, remove oldest one
    if (numPoints > pointCount) {
	data.removeRow(0);
	numPoints = data.getNumberOfRows();
    }
    // shuffle colors
    var jj =0;
    for (var ii=pointCount-numPoints; ii < pointCount; ii++) {
	style = 'point { size:'+ sizes[ii] +'; fill-color:'+ colors[ii] +'; }' ;
	data.setCell(jj, 2, style);
	jj++;
    } 
    // redraw chart
    chart.draw(data, options);
}

// sets up chart
function drawChart() {
    data = new google.visualization.DataTable( );
    data.addColumn('number','x');
    data.addColumn('number','y');
    data.addColumn({'type': 'string', 'role': 'style'});
    chart = new google.visualization.ScatterChart(document.getElementById('chart'));
    chart.draw(data, options);
    //when we are getting real data, comment out next line
    setInterval(updateChart,2000);
}

// To get data from the server uncomment this
   var socket = io();
   socket.on('data', function(msg){
   console.log("got data: x",msg.x,"y",msg.y );
   updateChart(msg.x,msg.y);
   });

