# Two-Reaction-Wheels-Unicycle

ESP32, Nidec 24H motors, MPU6050, 3S 500 mAh LiPo battery.

The controller is the same one I used in the self-balancing cube. I use encoders, so the red connections in the schematic are important. If you've made a self-balancing cube before, you can transform it into a unicycle.

Balancing controllers can be tuned remotely over bluetooth (but do this only if you know what you are doing).

Example (change K1 for real wheel):

Send p+ (or p+p+p+p+p+p+p+) for increase K1.

Send p- (or p-p-p-p-p-p-p-) for decrease K1.

Turn can test by send m+ or m-.

Remote control over Joy BT Commander app.

<img src="/pictures/foto1.jpg" alt="Self balancing unicycle"/>

First connect to controller over bluetooth. You will see a message that you need to calibrate the balancing point. Send c+ from serial monitor. This activate calibrating procedure. Set the unicycle to balancing point. Hold still when the unicycle does not fall to either side. Send c- from serial monitor. This will write the offsets to the EEPROM. After calibrating, the unicycle will begin to balance.

More about this:

https://youtu.be/gByWvAQIvNI

If something doesn't work, you can try test sketch. This sketch tests all components. Follow the messages in the serial monitor.