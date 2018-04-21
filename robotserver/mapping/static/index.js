//x and y are original map points from server
//map, x, and y must be the same size
//should not be modified outside the server
var x = []
var y = []

//map is the scaled version of x and y that will be drawn onto the canvas
var map = []

//xInput and yInput are values of the blue dot (wherever user clicks on the canvas)
var xInput, yInput = null;

//patrolPathScale is the scaled version of the patrol path that is drawn to the canvas
var patrolPathScale = []
//patrolPath is the unscaled path (coordinates sent to the robot)
var patrolPath=[]

//position of the robot. coord is scaled while coordOG is not scaled (directly from server)
var coord = [null, null];
var coordOG = [null, null];
var angle = null;

//POINT TO MOVE TO, represented by blue dot on map
var blueCoord = [0,0];

//Variables for server communication
var map_out_of_date = true
var currently_mapping = false
var waiting_for_click = false

var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");

//information for scaling
var mapScale;

// updated when scaleMap is called.
var xShift;
var yShift;

initialize();

checkForLocation();

//MAP MUST BE FULLY SET UP BEFORE LOCATION CAN BE SENT

/////////////////////////////////////////////////////////////////////////////////


function initialize() {

    drawMap();

    //register coordinates when canvas is pressed, draw a blue dot at position
    canvas.addEventListener("mousedown", updatePoint, false);

}

function checkForLocation(){
    $.get('API/current_location', function(data){
        var current_location = data["current_location"];

        if(!currently_mapping){
            coordOG[0] = current_location["x"];
            coordOG[1] = current_location["y"];
            angle = current_location["angle"];
            scaleCoord();
            drawMap();

            //write current location
            document.getElementById("pcXY").innerHTML =
                "x = " + coordOG[0] + " y = " + coordOG[1];
        }

        //call itself again after half a second
        setTimeout(checkForLocation, 500);
    })
}

//adds a coordinate to patrol path when add coordinate button is pressed
function addXY_PathBTN(){

    var unscaleXcoord = unscaleX(xInput);
    var unscaleYcoord = unscaleY(yInput);

    var inside = inPoly(xInput, yInput, map);
    if(inside == true){
        patrolPathScale.push([xInput, yInput]);
        patrolPath.push([unscaleXcoord ,unscaleYcoord]);
        //scalePath();
        drawMap();

        document.getElementById("addCoordSuccess").innerHTML = "Successfully added!";
    }
    else{
        document.getElementById("addCoordSuccess").innerHTML = "Out of bounds!";
    }
}

//draws arrow from current position point (red) at given angle
function drawArrow(angle_x){
    var fromx = coord[0];
    var fromy = coord[1];

    var tox = coord[0] + 50*Math.cos(angle_x*Math.PI/180);
    var toy = coord[1] - 50*Math.sin(angle_x*Math.PI/180);

    //variables to be used when creating the arrow
    var c = document.getElementById("myCanvas");
    var ctx = c.getContext("2d");
    var headlen = 10;

    var angle = Math.atan2(toy-fromy,tox-fromx);

    //starting path of the arrow from the start square to the end square and drawing the stroke
    ctx.beginPath();
    ctx.moveTo(fromx, fromy);
    ctx.lineTo(tox, toy);
    ctx.strokeStyle = "#cc0000";
    ctx.lineWidth = 2;
    ctx.stroke();

    //starting a new path from the head of the arrow to one of the sides of the point
    ctx.beginPath();
    ctx.moveTo(tox, toy);
    ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/7),toy-headlen*Math.sin(angle-Math.PI/7));

    //path from the side point of the arrow, to the other side point
    ctx.lineTo(tox-headlen*Math.cos(angle+Math.PI/7),toy-headlen*Math.sin(angle+Math.PI/7));

    //path from the side point back to the tip of the arrow, and then again to the opposite side point
    ctx.lineTo(tox, toy);
    ctx.lineTo(tox-headlen*Math.cos(angle-Math.PI/7),toy-headlen*Math.sin(angle-Math.PI/7));

    //draws the paths created above
    ctx.strokeStyle = "red";
    ctx.lineWidth = 2;
    ctx.stroke();
    ctx.fillStyle = "red";
    ctx.fill();
}


//draw dot at xpos, ypos on the canvas of a specified color
function drawCoord(xpos, ypos, color) {
    ctx.arcStyle = color;
    ctx.beginPath();
    ctx.arc(xpos, ypos, 5, 0, 2 * Math.PI, true);
    ctx.stroke();
    ctx.fillStyle = color;
    ctx.fill();
}

//draws the patrol path in white
function drawPatrolPath(){
    for(i = 0; i<patrolPathScale.length; i++){
        drawCoord(patrolPathScale[i][0], patrolPathScale[i][1], "white");
    }


    //connect the dots with a white line
    ctx.strokeStyle = "white";
    for (i = 1; i < patrolPathScale.length; i++) {
        ctx.moveTo(patrolPathScale[i - 1][0], patrolPathScale[i - 1][1]);
        ctx.lineTo(patrolPathScale[i][0], patrolPathScale[i][1]);
        ctx.stroke();
    }
}


//draws map
function drawMap() {
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, 1000, 1000);
    ctx.strokeStyle = "green";
    ctx.beginPath();

    //SERVER RELATED STUFF
    var url = (currently_mapping) ? "/API/stop_mapping" :
        (map_out_of_date) ? "/API/boundaries" :
            '';
    var type = (currently_mapping) ? "POST" :
        (map_out_of_date) ? "GET" :
            '';

    var request = null;

    //retrieve values from server if out of date or if currently mapping
    if ((currently_mapping && !waiting_for_click) || map_out_of_date){
        request = $.ajax({
            type: type,
            url: url,
            dataType: "json",

            //draws the map using points
            success: function(data){
                //for each data point in data["boundaries"], add them to the respective x and y arrays
                for (var i = 0; i < data["boundaries"].length; i++) {
                    x.push(data["boundaries"][i]['x']);
                    y.push(data["boundaries"][i]['y']);
                }

                //add the initial point twice to complete the loop
                x.push(x[0])
                y.push(y[0])
            }
        });
    }

    if(request != null){
        request.done(function(){
            console.log('finished gathering from server');
            currently_mapping = false;
            map_out_of_date = false;
            mapScale = getScale(x,y);
            scaleMap();
            scaleCoord();

            console.log('about to draw after making request');
            //draws map
            for (i = 1; i < map.length; i++) {
                ctx.moveTo(map[i - 1][0], map[i - 1][1]);
                ctx.lineTo(map[i][0], map[i][1]);
                ctx.stroke();
            }
            drawCoord(coord[0], coord[1], "red");
            drawArrow(angle);
            drawPatrolPath();
            //draws blue dot , check if it is inside boundary
            if(xInput != null && yInput != null){

                var inside = inPoly(xInput, yInput, map);
                if (inside == true) {
                    drawCoord(xInput, yInput, "blue");

                    blueCoord[0] = unscaleX(xInput);
                    blueCoord[1] = unscaleY(yInput);

                    document.getElementById("pXY").innerHTML = "x = " + Math.round(blueCoord[0]) + " y = " + Math.round(blueCoord[1]);
                    document.getElementById("pXY").style.color = "white";
                } else {
                    //put a green dot where clicked, notify user point is out of bounds
                    drawCoord(xInput, yInput, "green");
                    document.getElementById("pXY").innerHTML = "out of bounds";
                    document.getElementById("pXY").style.color = "red";
                }
            }
        });

    } else {
        console.log('about to draw without making request');
        //draws map
        for (i = 1; i < map.length; i++) {
            ctx.moveTo(map[i - 1][0], map[i - 1][1]);
            ctx.lineTo(map[i][0], map[i][1]);
            ctx.stroke();
        }
        drawCoord(coord[0], coord[1], "red");
        drawArrow(angle);
        drawPatrolPath();

        //check if point is inside the boundaries and draw a dot where it is clicked
        if(xInput != null && yInput != null){

            var inside = inPoly(xInput, yInput, map);
            if (inside == true) {
                drawCoord(xInput, yInput, "blue");

                blueCoord[0] = unscaleX(xInput);
                blueCoord[1] = unscaleY(yInput);

                document.getElementById("pXY").innerHTML = "x = " + Math.round(blueCoord[0]) + " y = " + Math.round(blueCoord[1]);
                document.getElementById("pXY").style.color = "white";
            } else {
                drawCoord(xInput, yInput, "green");
                document.getElementById("pXY").innerHTML = "out of bounds";
                document.getElementById("pXY").style.color = "red";
            }
        }
    }

}

//called when blue dot is relocated
function updatePoint(event) {

    var rect = canvas.getBoundingClientRect();
    xInput = event.clientX - rect.left;
    yInput = event.clientY - rect.top;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    console.log("about to set waiting for click to false")
    waiting_for_click = false;
    //redraw
    drawMap();



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
//1. make everything POSITIVE by adding minX/Y IF minX/Y is NEGATIVE
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
        map[i] = []
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
//get minimum of array xm
function getMin(xm) {
    var min = xm[0];
    for (i = 0; i < xm.length; i++) {
        if (xm[i] < min) {min = xm[i];}
    }
    return min;
}

//get maximum of array xM
function getMax(xM) {
    var max = xM[0];
    for (i = 0; i < xM.length; i++) {
        if (xM[i] > max) {max = xM[i];}
    }
    return max;
}

//Algorithm:
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
//REVERSE SCALE ALGORITHM STEPS: 4->1
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
//REVERSE SCALE ALGORITHM STEPS: 4->1
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

//scales path
//scaling algorithm:
//1. make everything POSITIVE by adding minX/Y IF NEGATIVE
//2. multiply map by scale
//3. y = 800-y
//4. shift everything to center
function scalePath() {
    var xs=[];
    var ys=[];

    for(i = 0; i<patrolPath.length; i++){
        xs[i] = patrolPath[i][0];
        ys[i] = patrolPath[i][1];
    }

    var minX = getMin(xs);
    var minY = getMin(ys);

    var scale = mapScale;

    //assigns scaled path values
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
        patrolPathScale[i] = []
        patrolPathScale[i][0] = xs[i];
        patrolPathScale[i][1] = 800 - ys[i];
    }

    //shifts the graph up/down
    minX = getMin(xs);
    var maxX = getMax(xs);

    minY = getMin(ys);
    var maxY = getMax(ys);

    xShift = (800 - (maxX - minX) )/2;
    yShift = (800 - (maxY - minY) )/2;

    for(i = 0; i < xs.length; i++){
        patrolPathScale[i][0] += xShift;
        patrolPathScale[i][1] -= yShift;
    }
}


///////////////////////FOR SERVER TO COMPLETE///////////////////////////
function downloadMap(){
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, 1000, 1000);
    ctx.fillStyle = "green";
    ctx.font = "50px Arial";

    ctx.fillText("Creating map",50,300);
    ctx.fillText("Click to stop mapping :)", 50, 400);
    //download map to display

    //set the out of date flag to true
    currently_mapping = true
    waiting_for_click = true
    console.log("set waiting for click to true")
    //clear x and y and map
    x = []
    y = []
    map = []

    //make request to start mapping
    $.post("API/start_mapping");

}

function sendMoveToXY() {
    //send blueCoord array values to SERVER
    $.post("API/current_target", {
        "x": blueCoord[0],
        "y": blueCoord[1]
    }, 'json');
}

function startPatrolling(){
    $.post("API/start_patrolling", {
        "patrol_path": JSON.stringify(patrolPath)
    }, 'json');
}

function stopPatrolling(){
    $.post("API/stop_patrolling");
}

function clearPath(){
    $.post("API/clear_patrol_path");
    patrolPath = [];
    patrolPathScale = [];
    drawMap();
}


/*
 * stuff for jquery
 */
$('#add_point_form').submit(function(e){
    e.preventDefault();
    var values = $(this).serializeArray();
    $.post("API/boundaries", {
        "x": values[0]['value'],
        "y": values[1]['value']
    }, 'json');
})