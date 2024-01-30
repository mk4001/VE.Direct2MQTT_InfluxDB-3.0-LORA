Here you will find the Arduino suite necessary for the InfluxDB/WiFi GATEWAY.

Inside the Sketch you will have to specify all the credentials for connecting to InfluxDB 2.0 Cloud, i.e.:

(EXAMPLE)

the InfluxDB 2.0 Cloud site URL for your country:

[influxDBUrl = "https://eu-central-1-1.aws.cloud2.influxdata.com"]

The TOKEN that you will have generated in the InfluxDB 2.0 console

[influxDBToken = "kjadshkjh32786iwefgkbkvu08718327309rgfkh2378t90fig23hohwgvwo=="]

The numerical value relating to your ORGANIZATION that you will find in the InfluxDB 2.0 console relating to your username

[influxDBOrg = "dkasjhkd87a6s868f76"]

and finally the name of the BUCKET you intend to use to write the collected data.

[influxDBBucket = "Victron"]

The GATEWAY will be placed in your home and must be connected to your home WiFi by specifying the necessary credentials directly in the sketch.

obviously you can choose to have InfluxDB on Premise, perhaps on a Raspberry PI, why not...

In this case it will be sufficient to replace the Cloud credentials with those of the on-premise version
