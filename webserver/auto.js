var SerialPort = require("serialport").SerialPort
var serialPort = new SerialPort(process.argv[2], {
  baudrate: 9600
});

var express = require('express');
var app = express();
var server = require('http').Server(app);
var io = require('socket.io')(server);

server.listen(80);
app.use(express.static(__dirname));

serialPort.on("open", function () {
  io.on('connection', function(socket){
    console.log('a user connected');

    socket.on('send', function(data) {
      console.log('start');
      serialPort.write(data);
    });
  });

  console.log('serial port opened');
  serialPort.on('data', function(data) {
    console.log('data received: ' + data);

  });

});

