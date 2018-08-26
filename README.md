RobotAka Head Tracker v1.0
---

Based on Razor AHRS (https://github.com/Razor-AHRS/razor-9dof-ahrs) with modifications from Furax49 (https://sourceforge.net/u/furax49/profile/).

Download
---

Clone the [repository on GitHub](https://github.com/RobotAka/RobotAka-Head-Tracker).

Quick setup
---

- Install Platformio (https://platformio.org/platformio-ide) or Arduino IDE (https://www.arduino.cc/en/Main/Software).
- Get Arduino Nano and GY-85 sensor.
- Connect board and sensor:

    A4 <--> SDA  
	A5 <--> SCL  
    GND <--> GND  
    VCC_IN <--> +5V

- Build and upload project to board.
- Install opentrack (https://github.com/opentrack/opentrack).
- Launch opentrack and select `Hatire Arduino` as input and `freetrack 2.0. Enhanced` as output.
- Open `Hatire Arduino` configuration and setup in following way:

    - General/Serial port: your COM port (can be found in Windows devices list from Settings)
	- Command:
	  - Init: #oh (delay 3000)
	  - Start: #o1 (delay 2000)
	  - Delay after startup: 1000
	  - Stop: #o0
	  - Center: #oz
	  - Zero: #oZ (delay 10)
	  - Baud rate: 57600
	  - Data bits: 8
	  - Parity: None
	  - Stop bits: 1
	  - Flow control: RTS/CTS

And that's it! You are ready to run games with head tracker control.
