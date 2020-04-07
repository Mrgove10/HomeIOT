const config = require('./config.json')
const mysql = require('mysql');
const mqtt = require('mqtt')
const client = mqtt.connect(config.mqtt_ServerIp)


// create the connection to database
var connection = mysql.createConnection({
    host: config.db_adress,
    user: config.db_user,
    password: config.db_password,
    database: config.db_database
});

connection.connect();

//connection.end();

client.on('connect', function () {
    client.subscribe(config.mqtt_channelId_inside, function (err) {
        if (err) { console.log(err) }
    })
    /* client.subscribe(config.mqtt_channelId_outside, function (err) {
         if (err) { console.log(err) }
     })*/
})

client.on('message', function (topic, message) {
    //Inside Information
    if (topic === config.mqtt_channelId_inside) {
        // message is Buffer
        let msg = message.toString();
        msg = JSON.parse(msg);
        console.log(msg);
        AddtoDatabaseInside(msg.t_in, msg.p, msg.h_in);
    }

    //Outside information
    if (topic === config.mqtt_channelId_outside) {
//TODO Free to implement here
    }
})

AddtoDatabaseInside = function (tmpin, pressure, humidin) {
    var currentdate = new Date().toISOString().slice(0, 19).replace('T', ' ');
    connection.query(`INSERT INTO Stats (Date, InsideTemp, Pressure, InsideHumidity) 
      VALUES ('${currentdate}', '${tmpin}', '${pressure}', '${humidin}')`,
        function (error, results, fields) {
            if (error) throw error;
            console.log(results);
        });
}

