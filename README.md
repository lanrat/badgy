# Badgy
A HID ID badge collector

This is a battery powered self contained HID badge collector. It it meant to sit attached to a wall next to a door and collect scanned badge codes to be replayed at a later date. As a bonus collected badge codes are transmitted over FM 103.3 in hex using espeak.

<img alt="Badgy Front" src="https://cloud.githubusercontent.com/assets/164192/16549955/dbc60ba2-4159-11e6-97b2-052cf87bb57e.jpg" width="50%">
<img alt="Badgy Back" src="https://cloud.githubusercontent.com/assets/164192/16549958/e03c4016-4159-11e6-8fc0-2cc0d002265d.jpg" width="50%">

More information can be found on my [Blog](https://lanrat.com/badgy/).

## Demo Video
[![Demo Video](https://img.youtube.com/vi/GYZLhhPMKZM/0.jpg)](https://www.youtube.com/watch?v=GYZLhhPMKZM)

## Construction

* Raspberry Pi Zero
* ICLASS HID Reader
* USB Battery Pack
* Wire for FM radio transmitter

![Badgy Inside](https://cloud.githubusercontent.com/assets/164192/16549959/e27bccca-4159-11e6-986b-16829e262aca.jpg)


# Arduino
The arduino folder contains sketches to have an Arduino or Teensy emulate a badge reader to send collected badge codes to the badge receiver using the Wiegand protocol.
The folder also contains 2 programs that can allow an Arduino to interface with the HID badge reader to read badges in place of a Pi.