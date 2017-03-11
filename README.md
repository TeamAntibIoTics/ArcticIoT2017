#AntibIoTics code repository for the Arctic IoT 2017
The AntibIoTics solution is a climate and sleep monitoring solution, integration sensor components with the cloud for a BigData, scalable architecture, leveraging many of the central components of Azure.

The sleep monitoring part uses a X2M200 Novelda sensor, recording the slightest movement by Doppler-radar technology. It is connected by USB to a device (either Win IoT Core/Linux) where the C++ application XeThru samples and records the events as single files.
The climate monitoring solution uses Silicon Labs Thunderboard Sense to sample data regarding temperature, humidity, carbon dioxide, sound level and light. Both sensor systems have its data pushed to the cloud where Azure Event Hubs functions as the data ingestion component. From here the data flow is diverged to cold and hot paths, i.e blob, table storage and a streaming dataset connected to PowerBI.

The solution also consists of an Azure ML-instance where sleep data is trained and can be used to correlate and later get a quality measure of the last nights sleep.

## AntibIoTics.DataModel
Contains the DTO-classes for communicating between the devices and cloud solutions. The project is compiled as a UWP-library.

## AntibIoTics Thunderboard
The UWP-application designed to run on Windows 10 or Windows IoT Core. Contains logic for discovering a Thunderboard Sense device and sample the sensordata it provides to the cloud. Utilizes Azure EventHub-client functionality to push the data.

## AntibIoTics XeThru
A C++-application which collects sleep-monitoring data from a XeThru X2M200 sensor module. The application saves the events in individual CSV-files at a designated directory.

## AntibIoTics xm200Pusher
A Node.js application that scans a directory for sleep-monitoring event data and pushes them by Azure EventHub-client functionality to the cloud.

## AntibIoTics Azure ML Sleepdata
Coming soon!

## AntibIoTics Azure Resource template
Coming soon!

## README.md
This file
