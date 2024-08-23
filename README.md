# Sprinkler Controller MCU

A simple controller for a sprinkler system that is flexible and (fairly) easily updatable.

## Hardware Setup

This assumes the use of a set of relays to control the zones and a real time clock to schedule when the sprinklers should be activated. Additional support for a phone app and a simple, local UI is planned, but not actively being developed.

## Software Setup

I've thought long and hard about the possibility of using a DDS protocol, since I like the fact that everything is decentralized... but it would ultimately be a fair amount of effort (since there's less free support than alternatives) for a small bit of redundancy

``` bash
git clone git@github.com:0Hogan/SprinklerControllerMcu
cd SprinklerControllerMcu
git submodule update --init --recursive
```

## Current Status

Still in active development. Don't expect anything to work in a refined way at this point in time, as it's just one person devoting a bit of time here and there to the project.

## Other thoughts

* Get rid of the dependency on ArduinoJSON in McuMqttInterface... It's just plain unnecessary.
  * Major advantage of JSON is flexibility, but ArduinoJSON loses some of the flexibility by not having reflection
  * Major downside to custom solution is compatibility issues and some increased complexity/decreased flexibility
    * Not a major decrease in flexibility considering the route I'm already going.
    * Compatibility issues can be handled via msg version or some other solution. Bottom line: They can be handled.
    * Increased complexity will take more time to implement, but I'll learn more and be able to simplify other aspects. (Maybe just use a std::map instead of JSON?)
    * Might be more efficient data-wise.
    * Could possibly lend itself better to auto-generating msg definitions...