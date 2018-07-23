/*
*    main.js
*/

var w = 500;
var h = 500;

var svg = d3.select("#visualisation-area")
    .append("svg")
        .attr("width", w)
        .attr("height", h);

d3.json("data/more-people.json", function(data){
    console.log(data);

    /*data.forEach(d => {
        d.height = +d.height;
    });*/

    var x = d3.scaleBand()
        .domain(data.map(function(d){
            return d.name;
        }))
        .range([0, w])
        .paddingInner(0.3)
        .paddingOuter(0.3);

    var y = d3.scaleLinear()
        .domain([0, d3.max(data, function(d){
            return d.height;
        })])
        .range([0, h]);
 

    var rects = svg.selectAll("rect")
        .data(data)
        .enter()
        .append("rect")
        .attr("y", function(d){
            return h - d.height;
        })
        .attr("x", function(d){
            return x(d.name);
        })
        .attr("width", x.bandwidth)
        .attr("height", function(d){
            return y(d.height);
        })
        .attr("fill", "grey");

})



