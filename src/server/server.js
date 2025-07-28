const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const dgram = require('dgram');
const path = require('path');

// Ports
const PORT = 8080;     // HTTP + WebSocket
const UDP_PORT = 3737; // UDP Empfang

// Express-Setup
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server }); // WebSocket über HTTP-Server

// Statische Datei ausliefern (index.html + ggf. CSS/JS)
app.use(express.static(path.join(__dirname)));

// WebSocket → an Clients senden
function broadcast(type, payload) {
    const message = JSON.stringify({ type, payload });
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message);
        }
    });
}

// UDP → empfangen
const udp = dgram.createSocket('udp4');
udp.on('message', msg => {
    const text = msg.toString();
    const sep = text.indexOf('|');

    if (sep === -1) {
        console.warn("Malformed message:", text);
        return;
    }

    const type = text.slice(0, sep);
    const payload = text.slice(sep + 1);

    try {
        const json = JSON.parse(payload);
        broadcast(type, json);
    } catch (e) {
        console.warn("Invalid JSON payload:", payload);
    }
});

udp.bind(UDP_PORT, () => {
    console.log(`UDP server listening on port ${UDP_PORT}`);
});

server.listen(PORT, () => {
    console.log(`HTTP/WebSocket server ready at http://localhost:${PORT}`);
});
