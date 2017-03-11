# AntibIoTics code repository for the Arctic IoT 2017

## AntibIoTics.DataModel
Contains the DTO-classes for communicating between the devices and cloud solutions. The project is compiled as a UWP-library.

## AntibIoTics Thunderboard
The UWP-application designed to run on Windows 10 or Windows IoT Core. Contains logic for discovering a Thunderboard Sense device and sample the sensordata it provides to the cloud. Utilizes Azure EventHub-client functionality to push the data.

## AntibIoTics XeThru
A C++-application which collects sleep-monitoring data from a XeThru X2M200 sensor module. The application saves the events in individual CSV-files at a designated directory.

## AntibIoTics xm200Pusher
A Node.js application that scans a directory for sleep-monitoring event data and pushes them by Azure EventHub-client functionality to the cloud.

## README.md
This file
