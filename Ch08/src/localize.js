google.charts.load('current', {'packages':['corechart']});
google.charts.setOnLoadCallback(drawChart);

var data;
var chart;
var options = {
    hAxis: {
	minValue: 0,
	maxValue: 8,
	textPosition: 'none',
	baselineColor: 'white',
	gridlines: {count: 0},
	ticks: 1
    },
    vAxis: {
	minValue: 0,
	maxValue: 11,
	textPosition: 'none',
	baselineColor: 'white',
	gridlines: {count:0},
	ticks: 1
    },
    legend: 'none',
    backgroundColor: 'transparent',
    chartArea: {width: '100%', height: '100%'}
};

// given a new point, update chart
function updateChart(x,y) {
    // add new point
    var style = 'point { size: 10; fill-color: #4a148c; }' ;
    var point=[ x , y , style ];
    data.addRow([ x, y, style ]);
    // if too many points, remove oldest one
    var numPoints = data.getNumberOfRows();
    if (numPoints > 1) data.removeRow(0);
    // redraw chart
    chart.draw(data, options);
}

// sets up chart
function drawChart() {
    data = new google.visualization.DataTable( );
    data.addColumn('number','x');
    data.addColumn('number','y');
    data.addColumn({'type': 'string', 'role': 'style'});
    chart = new google.visualization.ScatterChart(document.getElementById('plot'));
    chart.draw(data, options);
}

// To get data from the server uncomment this
   var socket = io();
   socket.on('data', function(msg){
   console.log("got data: x",msg.x,"y",msg.y );
       updateChart(msg.x,msg.y);
   });

