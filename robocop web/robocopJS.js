//map, x, and y must be the same size!!!!
//first 2 points and last point of x, y must be the same!!!
var x = [0, 0, 5000, 10000, 10000, 3000, 0, -300, -50, -50, 0];
var y = [3000, 3000, 3000, 500, -1000, -100, 0, 300, 8000, 300, 3000];

//map -must start with 0,0 and end with 0,0
var map = [[0,0], [0,0],[100,30],[300,30],[300,40],[200,40],[200, 60],[-30,-40],[-50,-60],[-50,-80],[0,0]];

//position of the robot
var coord = [500, 500];

scaleMap(x, y);

var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");

drawMap();

//register coordinates when canvas is pressed, draw a dot at position
canvas.addEventListener("mousedown", updateMap, false);

//write current location
document.getElementById("pcXY").innerHTML =
    "x = " + coord[0] + " y = " + coord[1];

//updates when location changes
drawCoord(coord[0], coord[1], "red");

//draw robot position
function drawCoord(x, y, color) {
    ctx.arcStyle = color;
    ctx.beginPath();
    ctx.arc(x, y, 5, 0, 2 * Math.PI, true);
    ctx.stroke();
    ctx.fillStyle = color;
    ctx.fill();
}

function drawMap() {
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, 1000, 1000);
    ctx.strokeStyle = "green";
    ctx.beginPath();
    for (i = 1; i < map.length; i++) {
        ctx.moveTo(map[i - 1][0], map[(i - 1, 1)]);
        ctx.lineTo(map[i][0], map[i][1]);
        ctx.stroke();
    }
}

function updateMap(event) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    //redraw
    drawMap();
    drawCoord(coord[0], coord[1], "red");

    var rect = canvas.getBoundingClientRect();
    var x = event.clientX - rect.left;
    var y = event.clientY - rect.top;

    var inside = inPoly(x, y, map);
    if (inside == true) {
        drawCoord(x, y, "blue");
        document.getElementById("pXY").innerHTML =
            "x = " + Math.round(x) + " y = " + Math.round(y);
        document.getElementById("pXY").style.color = "white";
    } else {
        drawCoord(x, y, "green");
        document.getElementById("pXY").innerHTML = "out of bounds";
        document.getElementById("pXY").style.color = "red";
    }
}

function inPoly(x, y, arr) {
    var cornersX = [];
    var cornersY = [];

    for (i = 1; i < arr.length; i++) {
        cornersX[i - 1] = arr[i][0];
        cornersY[i - 1] = arr[i][1];
    }

    var i,
        j = cornersX.length - 1;
    var oddNodes = false;

    var polyX = cornersX;
    var polyY = cornersY;
    for (i = 0; i < cornersX.length; i++) {
        if (
            ((polyY[i] < y && polyY[j] >= y) || (polyY[j] < y && polyY[i] >= y)) &&
            (polyX[i] <= x || polyX[j] <= x)
        ) {
            oddNodes ^=
                polyX[i] +
                (y - polyY[i]) / (polyY[j] - polyY[i]) * (polyX[j] - polyX[i]) <
                x;
        }
        j = i;
    }

    return oddNodes;
}


function scaleMap(xs, ys) {
    var minX = getMin(xs);
    var maxX = getMax(xs);

    var minY = getMin(ys);
    var maxY = getMax(ys);

    //750 is canvas size - 50 for margin
    var scalex = 600 / (maxX - minX);
    var scaley = 600 / (maxY - minY);

    var scale = Math.min(scalex, scaley);

    //assigns map values
    for (i = 0; i < xs.length; i++) {
        //turn everything positive
        if(minX < 0)
            xs[i]+=Math.abs(minX);
        if(minY < 0)
            ys[i]+=Math.abs(minY);

        xs[i] *= scale;
        ys[i] *= scale;
        map[i][0] = xs[i];
        map[i][1] = 800 - ys[i];
    }

    //shifts the graph up/down
    minX = getMin(xs);
    maxX = getMax(xs);
    minY = getMin(ys);
    maxY = getMax(ys);

    var xShift = (800 - (maxX - minX) )/2;
    var yShift = (800 - (maxY - minY) )/2;

    for(i = 0; i<x.length; i++){
        map[i][0] += xShift;
        map[i][1] -= yShift;
    }
}

function getMin(xm) {
    var min = xm[0];
    for (i = 0; i < xm.length; i++) {
        if (xm[i] < min) min = xm[i];
    }
    return min;
}

function getMax(xM) {
    var max = xM[0];
    for (i = 0; i < xM.length; i++) {
        if (xM[i] > max) max = xM[i];
    }
    return max;
}