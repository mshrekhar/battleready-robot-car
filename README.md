Battle Car Project
This project showcases our team's simple three-wheeled battle car, built for a competitive arena environment. The car features both manual and basic autonomous navigation modes:

![WhatsApp Image 2025-05-12 at 14 11 08_e4b73b35](https://github.com/user-attachments/assets/c0faf7d5-30ac-475a-991d-d5aa061d5639)

Hardware:
1. Front-facing PSD sensor
2. Side-mounted ultrasonic sensor
3. Vive Sensor - 2 mounted on the top most layer
4. ESP32S2 microcontroller charged by a portable 5V charger
5. Motors powered independently using NiMH batteries

Steering:

Manual: Web-based remote control interface

Autonomous: Rudimentary navigation algorithm (future work will enhance autonomy)

Arena Features:
1. LED ring and whisker switch integrated for game rules compliance
2. Vive setup available, but sensors proved unreliable near arena edges, so were rarely used

Note: Motors do not include encoders (no PID controller implemented); speed control worked well without it, but feel free to enhance with feedback control if needed

The project was built in the Arduino IDE environment. All the header files have been provided for this project in the folder. Please ensure that all files are in one folder when running the main file. 

Instructions:
1. Upload main file to Microcontroller.
2. Connect to the wifi network you named in webserver.cpp
3. Ensure to manually define the IP address you want to use for your remote control. This is also possible in the setupWebServer() function in webserver.cpp. Open your browser and enter this IP address as your URL.
4. You should now be able to switch between the manual and autonomous modes. Enjoy playing around with this code now!


