var request = require('request');
var fs = require('fs');
var file = __dirname + '/config.json';

var Broker = {};

var io=app.io;

Broker.subscribe = function(){
    fs.readFile(file, 'utf8', function (err, data) {
        if (err) {
            console.log('Error: ' + err);
            return;
        }

        var config = JSON.parse(data).broker;
        var body=JSON.stringify(config.request.body);
        var uri = config.scheme + '://' + config.host + ':' + config.port + '/v1/queryContext';
        var mimeType='application/json';
        io.on('connection', function (socket) {
            setInterval(function(){
                request({
                    method: 'POST',
                    uri: uri,
                    headers:{
                        'Content-Type': mimeType, 
                        'Content-Length': body.length,
                        'Accept': mimeType
                    },
                    body: body
                }, function (error, response, body) {
                    app.arduino(JSON.parse(body).contextResponses[0].contextElement.attributes[0].value);
                    if(response.statusCode == 200){
                        socket.emit('contextResponses', body);
                    } else {
                        console.log('error')
                        
                    }
                })
            },config.request.interval);
        });
    });
}

module.exports = Broker;