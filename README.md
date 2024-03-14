# Sprinkler Controller MCU

A simple controller for a sprinkler system that is flexible and (fairly) easily updatable.

## Hardware Setup

This assumes the use of a set of relays to control the zones and a real time clock to schedule when the sprinklers should be activated. Additional support for a phone app and a simple, local UI is planned, but not actively being developed.

## Software Setup

``` bash
git clone git@github.com:0Hogan/SprinklerControllerMcu
cd SprinklerControllerMcu
git submodule update --init --recursive
```