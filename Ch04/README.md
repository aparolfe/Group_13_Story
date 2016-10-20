## Synopsis

Create a system that will save data collected from a network of sensors and will allow visualization of data in both historical and real time views. Data should be hosted two ways: 
1. On the Particle cloud
2. Local version of Particle cloud on the RPi

## Installation & Setup

This challenge requires us to recreate [Challenge 2](https://github.com/aparolfe/Group_13_Story/tree/master/Ch02) using the [Particle Photon](https://www.particle.io/products/hardware/photon-wifi-dev-kit), [Particle CLoud](https://www.particle.io/products/platform/particle-cloud) and use Google Charts like services to display the data.

### Photon Particle

Installation and setup for Particle Photon, on how to connecct your Photon to the Wifi etc can be found on [the Particle docs](https://docs.particle.io/guide/getting-started/connect/photon/).

### ThingSpeak

[ThingSpeak](https://thingspeak.com/) is an platform that enables IoT and helps to collect, analyze and act on the data that is collected. 
We configure the Particle project to send data to ThingSpeak via [webhooks](https://docs.particle.io/guide/tools-and-features/webhooks/#how-webhooks-work) that are available on the Particle Cloud to send data to ThingSpeak.

### Webpage

While ThingSpeak allows data to be displayed, we created our own webpage to allow more flexibility to display the data. 

We use a NodeJS server to render a HTML page.

Live data update view

![alt text](https://github.com/aparolfe/Group_13_Story/blob/prerana/Ch04/static/liveupdates.png)

Historical data view
![alt text](https://github.com/aparolfe/Group_13_Story/blob/prerana/Ch04/static/historicalview.png)

## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan



