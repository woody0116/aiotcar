const buttonList = [
    document.getElementById("stop"),
    document.getElementById("up"),
    document.getElementById("down"),
    document.getElementById("left"),
    document.getElementById("right"),
    document.getElementById("upLeft"),
    document.getElementById("upRight"),
    document.getElementById("downLeft"),
    document.getElementById("downRight"),
    document.getElementById("turnLeft"),
    document.getElementById("turnRight"),
];

const camera    = document.getElementById("camera");
const ShowImage = document.getElementById('stream');
const canvas    = document.getElementById("canvas");
const object    = document.getElementById('object');
const score     = document.getElementById("score");
const count     = document.getElementById('count');

const cameraIP  = "192.168.30.158";

let websocket;
let websocketCamera;

let context     = canvas.getContext("2d");

let lastValue = "";
let restartCount = 0;
let Model;
let angle1Value1 = 4850;
let angle1Value2 = 4850;

window.addEventListener('load', onLoad);

function onLoad() {
    initWebSocket();
    initWebSocketCamera();
    ObjectDetect();

    console.log("object:", object.value);
    console.log("score:", score.value);
}

/*********************************** ESP32 ****************************************/

function initWebSocket() {
    console.log('[ESP32] Trying to open a WebSocket connection...');
    websocket           = new WebSocket("ws://" + window.location.hostname + "/ws");
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('[ESP32] Connection opened');
}

function onClose(event) {
    console.log('[ESP32] Connection closed');
    setTimeout(initWebSocketCamera, 2000);
}

function onMessage(event) {
    console.log("Msg[ESP32 DEV]:", event.data);
}

/*********************************** Camera ****************************************/

function initWebSocketCamera() {
    console.log('[ESP32 Camera] Trying to open a WebSocket connection...');
    websocketCamera           = new WebSocket("ws://" + cameraIP + "/Camera");
    websocketCamera.onopen    = onOpenCamera;
    websocketCamera.onclose   = onCloseCamera;
    websocketCamera.onmessage = onMessageCamera;
}

function onOpenCamera(event) {
    console.log('[ESP32 Camera] TConnection opened');
}

function onCloseCamera(event) {
    console.log('[ESP32 Camera] TConnection closed');
    setTimeout(initWebSocketCamera, 2000);
}

function onMessageCamera(event) {
    // console.log("Msg[ESP32 CAM]:", event.data);
    camera.src = URL.createObjectURL(event.data);
}

/*********************************** Object Recognition ****************************************/

camera.addEventListener("load", async () => {
    console.log("camera.onload");
    restartCount = 0;
    canvas.setAttribute("width", camera.width);
    canvas.setAttribute("height", camera.height);
    context.translate((canvas.width + camera.width) / 2, 0);
    context.scale(-1, 1);
    context.drawImage(camera, 0, 0, camera.width, camera.height);
    context.setTransform(1, 0, 0, 1, 0, 0);

    if (Model) {
        await DetectImage();
    }
});
  
function ObjectDetect() {
    cocoSsd.load().then(cocoSsd_Model => {
        Model = cocoSsd_Model;
        console.log("model loaded.");
    });
}

function sleep(milliseconds) 
{ 
var start = new Date().getTime(); 
while(1)
     if ((new Date().getTime() - start) > milliseconds)
          break;
     console.log("Sleeping.....");
}

async function DetectImage() {
    Model.detect(canvas).then(Predictions => {
        var s = (canvas.width > canvas.height) ? canvas.width : canvas.height;
        var objectCount = 0;  //紀錄偵測到物件總數
        var trackState = 0;  //是否已偵測到物件
        var x, y, width, height;

        //console.log('Predictions: ', Predictions);
        if (Predictions.length > 0) {
            result.innerHTML = "";
            for (var i = 0; i < Predictions.length; i++) {
                const x = Predictions[i].bbox[0];
                const y = Predictions[i].bbox[1];
                const width = Predictions[i].bbox[2];
                const height = Predictions[i].bbox[3];
                context.lineWidth = Math.round(s / 200);
                context.strokeStyle = "rgba(255, 111, 0, 0.85)";
                context.beginPath();
                context.rect(x, y, width, height);
                context.stroke();
                context.lineWidth = "2";
                context.fillStyle = "blue";
                context.font = Math.round(s / 30) + "px Arial";
                context.fillText(Predictions[i].class, x, y);
        
                if (Predictions[i].class == object.value && Predictions[i].score >= score.value && trackState == 0) {
                    trackState = 1;  //只偵測第一物件
                    console.log('Detected Object: ', object.value);
                    var midX = Math.round(x) + Math.round(width) / 2;  //取偵測到第一物件的區域水平中點X值
                    if (midX > (70 + 640 / 2)) {  //畫面中心自訂水平小區域中即使偏右不轉動
                        if (midX > 260) {  //區分距離畫面中點偏右程度
                            angle1Value1 -= 350;1
                        } else {         
                            angle1Value1 -= 175;
                        }
                        if (angle1Value1 > 7650) angle1Value1 = 7650;
                        if (angle1Value1 < 2050) angle1Value1 = 2050;
                        console.log("[Tensorflow] send: 3 Left Movement");
                        websocket.send(3);  
                        for (let i=1;i<=5000;i++)
                        {
                            console.log("Left Step:" + i);
                        }     
                        websocket.send(0);                
                    }
                    else if (midX < (640 / 2) - 70) {  //畫面中心自訂水平小區域中即使偏左不轉動
                        if (midX < 60) {  //區分距離畫面中點偏左程度              
                            angle1Value1 += 350;
                        } else {            
                            angle1Value1 += 175;
                        }
                        if (angle1Value1 > 7650) angle1Value1 = 7650;
                        if (angle1Value1 < 2050) angle1Value1 = 2050;
                        console.log("[Tensorflow] send: 4 Right Movement");
                        websocket.send(4);       
                        for (let i=1;i<=5000;i++)
                        {
                            console.log("Right Step:" + i);
                        }  
                        websocket.send(0);                
                    }

                    var midY = Math.round(y) + Math.round(height) / 2;  //取偵測到第一物件的區域垂直中點Y值
                    if (midY > (480 / 2 + 70)) {  //畫面中心自訂小區域中即使偏下不轉動
                        if (midY > 195) {  //區分距離畫面中點偏下程度
                            angle1Value2 -= 300;
                        } else {
                            angle1Value2 -= 150;
                        }
                        if (angle1Value2 > 7650) angle1Value2 = 7650;
                        if (angle1Value2 < 2050) angle1Value2 = 2050;
                        console.log("[Tensorflow] send: 2");
                        websocket.send(2); //後退
                        sleep(10);           
                        websocket.send(0);             
                    }
                    else if (midY < (480 / 2) - 70) {  //畫面中心自訂小區域中即使偏上不轉動
                        if (midY < 45) {  //區分距離畫面中點偏上程度
                            angle1Value2 += 300;
                        } else {
                            angle1Value2 += 150;
                        }
                        if (angle1Value2 > 7650) angle1Value2 = 7650;
                        if (angle1Value2 < 2050) angle1Value2 = 2050;
                        console.log("[Tensorflow] send: 1");
                        websocket.send(1); //前進
                        sleep(10);
                        websocket.send(0);
                  }
              }

              if (Predictions[i].class == object.value) {
                  objectCount++;
              }
            }
            count.innerHTML = objectCount;
        }
        else {
            count.innerHTML = "0";
        }

        lastValue = count.innerHTML;

    });
}

async function sleep(ms = 0) {
    return new Promise(r => setTimeout(r, ms));
}
  
/*********************************** button ****************************************/

for(let i = 0; i < buttonList.length; i++) {
    buttonList[i].addEventListener("mousedown", () => {
        websocket.send(i);
    });

    buttonList[i].addEventListener("mouseup", () => {
        websocket.send(0);
    });

    buttonList[i].addEventListener("touchstart", () => {
        websocket.send(i);
    });

    buttonList[i].addEventListener("touchend", () => {
        websocket.send(i);
    });
}
