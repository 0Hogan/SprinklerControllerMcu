# Sprinkler Controller MCU

A simple controller for a sprinkler system that is flexible and (fairly) easily updatable.

## Hardware Setup

This assumes the use of a set of relays to control the zones and will assume a real time clock to schedule when the sprinklers should be activated. Additional support for a phone app and a simple, local UI is planned, but not actively being developed.

## Software Setup

This project relies upon a couple of Arduino libraries:
  * [McuLogger](https://github.com/0Hogan/McuLogger)
  * [McuMqtt](https://github.com/0Hogan/McuMqtt)

``` bash
git clone git@github.com:0Hogan/SprinklerControllerMcu
```

## Current Status

Still in active development. Don't expect anything to work in a refined way at this point in time, as it's just one person devoting a bit of time here and there to the project.
