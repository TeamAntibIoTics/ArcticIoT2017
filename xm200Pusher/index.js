/*
* Azure event hub client, for pushing XM200 data from file to cloud
 */

const azureEventClient = require('azure-event-hubs').Client;
const fs = require('fs');
const csv = require('csv');
const guid = require('guid');

// XM200 data
const xm200dataPath = './xm200data';
let prevData = [];
let jsonLoad = [];

// Azure Event hub
const deviceId = 'XM200';
const eventHub = 'sleepdata';
const azureEventConnStr = '';

// Override
// Warn if overriding existing method
if (Array.prototype.equals)
    console.warn("Overriding existing Array.prototype.equals. Possible causes: New API defines the method, there's a framework conflict or you've got double inclusions in your code.");
// attach the .equals method to Array's prototype to call it on any array
Array.prototype.equals = function (array) {
    // if the other array is a falsy value, return
    if (!array)
        return false;

    // compare lengths - can save a lot of time 
    if (this.length != array.length)
        return false;

    for (var i = 0, l = this.length; i < l; i++) {
        // Check if we have nested arrays
        if (this[i] instanceof Array && array[i] instanceof Array) {
            // recurse into the nested arrays
            if (!this[i].equals(array[i]))
                return false;
        }
        else if (this[i] != array[i]) {
            // Warning - two different object instances will never be equal: {x:20} != {x:20}
            return false;
        }
    }
    return true;
}
// Hide method from for-in loops
Object.defineProperty(Array.prototype, "equals", { enumerable: false });

var push2Cloud = function () {
    // Loop through jsonLoad and push 2 cloud
    var client = azureEventClient.fromConnectionString('Endpoint=' + azureEventConnStr, eventHub);
    console.log('Starting transfer to cloud');
    for (let i = 0; i < jsonLoad.length; i++) {
        let load = JSON.stringify(jsonLoad[i]);
        console.log(`Sending #${i} to event hub. Load: ${load}`);
        client.createSender()
            .then(function (tx) {
                tx.on('errorReceived', function (err) {
                    console.log('Error in Azure Event Hub tx:');
                    console.log(err);
                });
                tx.send({ load });
                console.log(`Load sent`);
            })
    }
    console.log("JSON Load:" + jsonLoad.length + ". Clearing!");
    jsonLoad = [];
    console.log("JSON Load:" + jsonLoad.length + ". Cleared!!!!!");
}

var convert2Objects = function (headers, data) {
    var sleepModel = {};
    // Build sleepModel object
    sleepModel.deviceId = deviceId;
    sleepModel.eventId = guid.raw();
    for (let i = 0; i < data.length; i++) {
        sleepModel[headers[i].trim()] = data[i].trim();
    }
    jsonLoad.push(sleepModel);
}

var processFile = function (data) {
    csv.parse(data, { delimiter: ';' }, function (err, data) {
        if (err) {
            console.log('Parsing error');
            console.log(err)
        } else {
            let parsedData = data;
            console.log('Data length: ' + parsedData.length);
            console.log('Data similar: ' + prevData.equals(parsedData));
                console.log('Processing data!');
                let prevLength = prevData.length;
                let parsedLength = parsedData.length;
                let headers = parsedData[0];
                for (i = 1; i < parsedLength; i++) {
                    convert2Objects(headers, parsedData[i]);
                }
                console.log('JSON Load length: ' + jsonLoad.length);
                prevData = parsedData;
                if (jsonLoad.length > 0) {
                    push2Cloud();
                }
        }
    });
}

// Watch XM200 datafolder
fs.watch(xm200dataPath, { "recursive": true }, (eventType, filename) => {
    console.log(`Event type is: ${eventType}`);
    if (eventType == 'change' && filename) {
        console.log(`Filename provided: ${filename}`)
        let fileData = fs.readFile(xm200dataPath + '/' + filename, "utf-8", function (err, fileData) {
            if (err) throw err;
            processFile(fileData);
        });
    } else {
        console.log('No filename provided');
    }
});
console.log(`Watching folder: ${xm200dataPath}`);