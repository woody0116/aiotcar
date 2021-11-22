const img1 = document.getElementById("img1");

let websocket;

window.addEventListener('load', onLoad);

function onLoad() {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket("ws://" + window.location.hostname + "/Camera");
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log("Msg[ESP32]:", event.data);
    img1.src = URL.createObjectURL(event.data);
}