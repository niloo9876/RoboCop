//map, x, and y must be the same size!!!!
//first 2 points and last point of x, y must be the same!!!
//x and y are original map points from server
//should not be modified!!!
var x = [0, 0, 5000, 10000, 1000, 3000, 0, -3000, -50, -50, 0];
var y = [ 8000, 8000, 3000, 500, -4000, -100, 0, 300, 6000, 7000, 8000];

//map -must start with 0,0 and end with 0,0
//map is the scaled version, MODIFIED1
var map = [[0,0], [0,0],[100,30],[300,30],[300,40],[200,40],[200, 60],[-30,-40],[-50,-60],[-50,-80],[0,0]];

//position of the robot. coord is scaled while og is not scaled, MODIFIED
var coord = [0,8000];

//NOT MODIFIED, FROM SERVER!!
var coordOG = [0, 8000];


//POINT TO MOVE TO, represented by blue dot on map
var blueCoord = [0,0];

var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");

//information for scaling
var mapScale = getScale(x,y);

/// / updated when scaleMap is called.
var xShift;
var yShift;

//REMOVE WHEN USED WITH SERVER
scaleMap();
//REMOVE WHEN USED WITH SERVER
scaleCoord();
initialize();


//MAP MUST BE FULLY SET UP BEFORE LOCATION CAN BE SENT

/////////////////////////////////////////////////////////////////////////////////


function initialize() {

    //checks if map AND coord is downloaded, if so BOTH SHOULD BE SCALED
    if(map != null && coord != null) {
        drawMap();


        //register coordinates when canvas is pressed, draw a dot at position
        canvas.addEventListener("mousedown", updatePoint, false);

        //write current location
        document.getElementById("pcXY").innerHTML =
            "x = " + coordOG[0] + " y = " + coordOG[1];
        //updates when location changes
        drawCoord(coord[0], coord[1], "red");
    }

    else{
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = "black";
        ctx.fillRect(0, 0, 1000, 1000);
        ctx.fillStyle = "green";
        ctx.font = "50px Arial";

        ctx.fillText("Map needed, Click update map...",50,300);
    }
}



//draw robot position
function drawCoord(xpos, ypos, color) {
    ctx.arcStyle = color;
    ctx.beginPath();
    ctx.arc(xpos, ypos, 5, 0, 2 * Math.PI, true);
    ctx.stroke();
    ctx.fillStyle = color;
    ctx.fill();
}

//draws map
function drawMap() {
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, 1000, 1000);
    ctx.strokeStyle = "green";
    ctx.beginPath();
    for (i = 1; i < map.length; i++) {
        ctx.moveTo(map[i - 1][0], map[i - 1][1]);
        ctx.lineTo(map[i][0], map[i][1]);
        ctx.stroke();
    }
}

//called when blue dot is relocated
function updatePoint(event) {

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    //redraw
    drawMap();
    drawCoord(coord[0], coord[1], "red");

    var rect = canvas.getBoundingClientRect();
    var xB = event.clientX - rect.left;
    var yB = event.clientY - rect.top;

    var inside = inPoly(xB, yB, map);
    if (inside == true) {
        drawCoord(xB, yB, "blue");

        blueCoord[0] = unscaleX(xB);
        blueCoord[1] = unscaleY(yB);

        document.getElementById("pXY").innerHTML = "x = " + Math.round(blueCoord[0]) + " y = " + Math.round(blueCoord[1]);
        document.getElementById("pXY").style.color = "white";
    } else {
        drawCoord(xB, yB, "green");
        document.getElementById("pXY").innerHTML = "out of bounds";
        document.getElementById("pXY").style.color = "red";
    }
}

//determine whether a coordnate is inside a polygon
function inPoly(x, y, arr) {
    var cornersX = [];
    var cornersY = [];

    for (i = 1; i < arr.length; i++) {
        cornersX[i - 1] = arr[i][0];
        cornersY[i - 1] = arr[i][1];
    }

    var i, j = cornersX.length - 1;
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

//scaling algorithm:
//1. make everything POSITIVE by adding minX/Y IF NEGATIVE
//2. multiply map by scale
//3. y = 800-y
//4. shift everything to center
function scaleMap() {
    var xs=[];
    var ys=[];
    for(i = 0; i<x.length; i++){
        xs[i] = x[i];
        ys[i] = y[i];
    }

    var minX = getMin(xs);
    var minY = getMin(ys);

    var scale = mapScale;

    //assigns map values
    for (i = 0; i < xs.length; i++) {
        //turn everything positive
        if(minX < 0){
            xs[i]+=Math.abs(minX);
        }
        if(minY < 0){
            ys[i]+=Math.abs(minY);
        }
        xs[i] *= scale;
        ys[i] *= scale;
        map[i][0] = xs[i];
        map[i][1] = 800 - ys[i];
    }

    //shifts the graph up/down
    minX = getMin(xs);
    var maxX = getMax(xs);

    minY = getMin(ys);
    var maxY = getMax(ys);

    xShift = (800 - (maxX - minX) )/2;
    yShift = (800 - (maxY - minY) )/2;

    for(i = 0; i<x.length; i++){
        map[i][0] += xShift;
        map[i][1] -= yShift;
    }
}

function getMin(xm) {
    var min = xm[0];
    for (i = 0; i < xm.length; i++) {
        if (xm[i] < min) {min = xm[i];}
    }
    return min;
}

function getMax(xM) {
    var max = xM[0];
    for (i = 0; i < xM.length; i++) {
        if (xM[i] > max) {max = xM[i];}
    }
    return max;
}

//ALGO:
//get min/max/X/y
//600/difference for x scale and y scale, leave 100 on each side for margin/ shifting space
//return min of 2 scale
function getScale(xs,ys){
    var minX = getMin(xs);
    var maxX = getMax(xs);

    var minY = getMin(ys);
    var maxY = getMax(ys);

    //800 is canvas size - 100 for margin on each side (to center)
    var scalex = 600 / (maxX - minX);
    var scaley = 600 / (maxY - minY);

    //give smaller of 2 scale so it does not go out of bounds
    return Math.min(scalex, scaley);
}

//unscales to send location of blue dot (placed by user
//REVERSE: 4->1
//scaling algorithm:
//1. make everything POSITIVE by adding minX/Y IF NEGATIVE
//2. multiply map by scale
//3. y = 800-y
//4. shift everything to center x+shift, y - shift
function unscaleX(xvar){
    xvar = xvar - xShift;
    xvar = xvar / mapScale;

    var xMin = getMin(x);
    if(xMin < 0){
        xvar += xMin;
    }

    return xvar;

}
//unscales to send location of blue dot (placed by user
//REVERSE: 4->1
//scaling algorithm:
//1. make everything POSITIVE by adding minX/Y IF NEGATIVE
//2. multiply map by scale
//3. y new = 800-y old
//4. shift everything to center x+shift, y - shift
function unscaleY(yvar){
    yvar = yvar + yShift;

    yvar = 800 - yvar;
    yvar /= mapScale;

    var yMin = getMin(y);
    if(yMin < 0){
        yvar += yMin;
    }

    return yvar;

}

//unscales to send location of blue dot (placed by user
//scaling algorithm:
//1. make everything POSITIVE by adding minX/Y IF NEGATIVE
//2. multiply map by scale
//3. y new = 800-y old
//4. shift everything to center x+shift, y - shift
function scaleCoord() {

    coord[0] = coordOG[0];
    coord[1] = coordOG[1];

    var minX = getMin(x);
    var minY = getMin(y);

    var scale = mapScale;

    //turn everything positive
    if(minX < 0){
        coord[0]+=Math.abs(minX);
    }
    if(minY < 0){
        coord[1]+=Math.abs(minY);
    }

    coord[0] *= scale;
    coord[1] *= scale;
    coord[1] = 800 - coord[1];


    coord[0] += xShift;
    coord[1] -= yShift;

}


///////////////////////FOR SERVER TO COMPLETE///////////////////////////
function downloadMap(){
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, 1000, 1000);
    ctx.fillStyle = "green";
    ctx.font = "50px Arial";

    ctx.fillText("Creating map... Please wait...",50,300);
    //download map to display

    //1. fill x and y array
    //2. fill map array with xy value as place holders (mainly to get correct array size)
    //3. call scale map
    //4. fill coordOG array
    //5. call scale coord

}

function sendMoveToXY() {
    //send blueCoord array values to SERVER
    //tells robot where to go
    //should send location when relocate is pressed
}
