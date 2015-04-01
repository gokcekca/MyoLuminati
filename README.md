# MyoLuminati
Co-Authored by MichaelLukowski

Inspiration

Gesture and motion control allows for powerful and intuitive interaction with technology. Our vision was to merge the physical and virtual worlds in a visual medium through the power of the internet.
How it works

Data from the Myo armband is parsed, and state information is sent to the Arduino-connected LED strip via UDP over a WiFi network. A two-byte data packet is all that is sent, with the first byte representing a state command, and the second byte representing the ASCII representation of the luminosity of a particular channel (Red, Green, Blue).

The controls are as follows:

Relaxed Gesture (locked/unlocked):

    Hand up/down with palm facing up will increase/decrease all color luminosity
    Hand turn (think doorknob) will fade through colors.
    Hand above head - will flash colors!

SwipeIn/SwipeOut Gestures(unlocked):

    Will iterate through a list of colors

Fist Gesture while unlocked:

    Will turn all lights off.

Challenges I ran into

Calibrating the Myo armband for use was strenuous, but after some tweaking it works well in most situations. The Myo is also limited to five gestures, which prevented us from implementing as many features as we would like. There was a latency issue with the early TCP/HTTP implementation of the client-server protocol, and UDP was chosen as a better alternative for the application.
Accomplishments that I'm proud of

Not only can the device code be used for lighting the LED strip, but it can also be used for other hardware control projects. Since the Arduino simply accepts UDP packets and acts accordingly, there are limitless possibilities for creating and controlling an Internet of Things.
What I learned

Real-time communication over WiFi is generally pretty slow. The Myo armband requires a heavy amount of calibration, and will hopefully have a lot of the bugs worked out in the near future.
What's next for MyoLuminati

Using a second Myo armband, a greater degree of accuracy and a larger host of features can be achieved. The LED strip could also be embedded in clothing, which would be sure to create a marketable and frankly, awesome piece of partywear. Furthermore, the PC used to connect to Myo and send UDP packets could be replaced by an iPhone app for greatly increased portability. In another use case, the strips could simply be mounted on the wall as a piece of intuitively-controlled home-automation technology.
