const Gpio = require('onoff').Gpio;
const exec = require('child_process').exec
const SerialPort = require('serialport');
const fs = require('fs');
const express = require('express');
const app = express();
const port = new SerialPort('/dev/ttyACM0', () => {
console.log('Port Opened');
});

const parsers = SerialPort.parsers;

const parser = new parsers.Readline({
delimiter: ';'
});
var LED = new Gpio(18, 'out');

let data = 'null;null';


function sendSensorData(data){

	data['time'] = Date.now();
	console.log(data);

	/*fs.readFile('/home/pi/eleAvto/nodeServer/log.json', function (err, log) {
		var json = {};
		if(!err){
			var json = JSON.parse(log);
		}
		json.push(data);    
		fs.writeFile("/home/pi/eleAvto/nodeServer/log.json", JSON.stringify(json,null,2), function(err){
		  if (err) throw err;
		  console.log('The "data to append" was appended to file!');
		});
	});*/

	dataStatus = data;
	
}


let dataSet={};
let dataLength = 8;
let dataStatus = {};

function retrieveData(dt){
	if(!dt.includes(':') || dt.includes('j0')) return;
	

	data = dt.split(':');
	dataSet[data[0]] =  parseFloat(data[1]);

	if(data[0] == 'AkuVoltage'){
		if(Object.keys(dataSet).length == dataLength){
			sendSensorData(dataSet)
		}
		dataSet={};
	}

	LED.writeSync(1);
	setTimeout(function(){
		LED.writeSync(0);
	}, 100);
}



port.pipe(parser);
parser.on('data', retrieveData);
LED.writeSync(0);

app.get('/', (req, res) => {
	res.json((dataStatus));
});

app.get('/log', (req, res) => {
	fs.readFile('/home/pi/eleAvto/nodeServer/log.json', function(err, log){
		if(err){ 
			res.end();
			throw err;
		}
		res.json(JSON.parse(log));
	});
});

const server = app.listen(80, () => {
	console.log(`Express running → PORT ${server.address().port}`);
  });