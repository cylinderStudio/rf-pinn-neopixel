# rf-pinn-neopixel
I needed to tinker with some wearable functionality where multiple Arduinos could send signals or commands to one another in peer-to-peer fashion over a wireless connection. I had two "scout" Arduino boards on hand from the now defunct Pinoccio platform. They are small, have mesh radios built-in and run on 3.7v 550mAH lipo batteries, so they were ideal to build a quick prototype ahead of buying and exploring something like XBee radios with Lilypad boards, for example.

Without using the Pinoccio cloud platform (HQ), I simply wanted one of two scouts (Arduino with mesh radio only, no wi-fi) to be able to tell the other scout to run a simple LED animation on a small connected strand of Neopixels if they were both in range of one another on the same radio channel and part of the same group, and vice versa. In the initial commit, this happened automatically. Now, the logic calls for an attached button trigger a single Neopixel to temporarily light for push-button feedback while calling the Neopixel animation on the other board, and vice versa. 

If you have a set of these boards around, and want to do something similar, here are the steps I took to configure and run the scenario in absence of Pinoccio HQ or running the command line tool.

**1. Set up the boards for Arduino IDE development:** 

http://support.pinocc.io/hc/en-us/articles/201489204-Pinoccio-the-Arduino-IDE

**2. Deploy this sketch or write your own similar sketch to each board. Be sure to change these constants appropriately for each board:**

```
#define NUM_PIXELS 16         \\ the number of neopixels in your strand; remember the 3.7v Pinoccio board can only power about 5 or 6 individual pixels at the same time
#define OTHER_BOARD 2         \\ the scoutID number you plan to assign to the other board in your pair
```

**Used the Arduino serial monitor to configure each board separately.**

```
mesh.config(1,100,21)         \\ sets the scoutID to 1, the troopID to 100 and the channel to 21; use a different scoutID for each board 
mesh.setkey("pinocciolives")  \\ sets the mesh radio security key; this needs to be set using the same key on all boards you want participating in the same mesh network or peer group
```

Sometimes uploading a sketch to Pinoccio times out, and you have to power the board off, disconnect from USB and try again. Occasionally multiple times. If it happens multiple times, I've seen the boards lose their mesh settings and I've had to run mesh.config() and mesh.setkey() again.
