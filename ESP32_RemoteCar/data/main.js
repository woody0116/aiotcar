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
const cameraIP  = "192.168.30.158";

let websocket;
let websocketCamera;

window.addEventListener('load', onLoad);

function onLoad() {
    initWebSocket();
    initWebSocketCamera();
}

/*********************************** ESP32 ****************************************/

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket           = new WebSocket("ws://" + window.location.hostname + "/ws");
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocketCamera, 2000);
}

function onMessage(event) {
    console.log("Msg[ESP32 DEV]:", event.data);
}

/*********************************** Camera ****************************************/

function initWebSocketCamera() {
    console.log('Trying to open a WebSocket connection...');
    websocketCamera           = new WebSocket("ws://" + cameraIP + "/Camera");
    websocketCamera.onopen    = onOpenCamera;
    websocketCamera.onclose   = onCloseCamera;
    websocketCamera.onmessage = onMessageCamera;
}

function onOpenCamera(event) {
    console.log('Connection opened');
}

function onCloseCamera(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessageCamera(event) {
    console.log("Msg[ESP32 CAM]:", event.data);
    camera.src = URL.createObjectURL(event.data);
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
