/*
*    main.js
*/

// select the DIV, then add SVG (the main canvas)
var svg = d3.select("#visualisation-area").append("svg")
	.attr("width", 400)
	.attr("height", 400);

// to SVG, add circle
var circle = svg.append("circle")
	.attr("cx", 100)
	.attr("cy", 250)
	.attr("r", 70)
	.attr("fill", "grey");