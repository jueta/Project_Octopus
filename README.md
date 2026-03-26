Project Description


![alt text](docs/v3.JPG)

HARDWARE:

ESP32S3 Xiao Board + CAM
PIR sensor -> SB00322A-1 // OLD: AMS312
Battery
switch
case

GOAL FUNCTIONALITY:

Connect to local wifi through web-page to configure Telegram bot and account.

off mode (HW implemented) -> using the switch to ON/OFF the device.

Battery powered

3D printed case - artistic and adaptable

    Picture mode (Main function) -> reading PIR sensor and, case activated, take 3 pictures and send to email.

    filming mode -> this will require lots of energy to record, connect to wifi, upload and send email. Must happen rarely.

    scanning/sleeping mode -> PIR is a passive sensor so this must be in low energy mode.

online mode (future idea) -> be able to take pictures from webpage.

RGB led colours will indicate operation mode


CREATE FIRST BOT: open in telegram the following bots

BotFather -> for BOTtoken
IDBot -> for chatID

HW:
solder 2 100k ohms resistors as a voltage dividor to read the battery voltage at pin D0.

MAIN REF:
https://randomnerdtutorials.com/telegram-esp32-cam-photo-arduino/

Project references: 

Rui Santos
https://RandomNerdTutorials.com/esp32-cam-shield-pcb-telegram/

Brian Lough's Universal Telegram Bot Library: 
https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot



--------------- DOWN IS DEPRECATED ------------------

PI VERSION TUTORIALS:

https://randomnerdtutorials.com/esp32-cam-post-image-photo-server/

https://randomnerdtutorials.com/raspberry-pi-apache-mysql-php-lamp-server/

OUTDATED FUNCTIONALITY:

It's sending picture into a certain ip address (raspiberry pi). The raspberry Pi is connected to the same wifi network running a PHP server and storing the pictures. The idea is that esp32 can come back to sleep.

DEPRECATED 3D MODEL: v0.1
https://www.thingiverse.com/thing:7133622