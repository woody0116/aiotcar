const button1 = document.getElementById("button1");
const state   = document.getElementById("state");
const state1  = document.getElementById("state1");
let websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket("ws://" + window.location.hostname + "/ws");
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    state.innerHTML = "Connect";
}

function onClose(event) {
    console.log('Connection closed');
    state.innerHTML = "Disconnect";
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log('Message:', event.data);
}

button1.addEventListener("click", () => {
    if (state1.innerText == "ON") {
        state1.innerText = "OFF"
        button1.innerText = "ON"
    } else {
        state1.innerText = "ON"
        button1.innerText = "OFF"
    }
    websocket.send(state1.innerText);
});

button1.addEventListener("touchend", () => {
    if (state1.innerText == "ON") {
        state1.innerText = "OFF"
        button1.innerText = "ON"

    } else {
        state1.innerText = "ON"
        button1.innerText = "OFF"
    }

    websocket.send(state1.innerText);
});