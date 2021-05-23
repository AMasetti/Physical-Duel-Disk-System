# Masetti Physical Duel Disk

![Duel Disk](https://i.ytimg.com/vi/Hd7VtOQJtWs/hqdefault.jpg?sqp=-oaymwEjCPYBEIoBSFryq4qpAxUIARUAAAAAGAElAADIQj0AgKJDeAE=&rs=AOn4CLBZGzKAloBVMwPntoRvJ6EskObvMg)

This repo has everything to build a Duel Disk System that can both read and write NFC cards (ISO 14443A). The Disk also has 5 buttons for interacting with the Backend, two of which have long press actions. The connection to the PC is done via bluetooth (configured in another project TBD). The write operation is performed with the Duel Disk connected to a serial interface with a usb cable connected to the arduino directly. 

## Part List

The part list depends on the user since customization is highly possible at this point, but for the basics you'll need:
- Arduino of your choice
- 5 Buttons of your choice
- Lipo Battery of your choice
- Lipo Battery Charger
- On/Off Switch
- HC-05 Bluetooth Module
- [ISO 14443 NFC Stickers](https://es.wikipedia.org/wiki/ISO_14443) for the cards
- [Electhouse PN532 Module](https://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=90_93&products_id=2242)


## Write Mode

There are currently two modes in the Duel Disk, one is for dueling an the other is for writing cards. The later will be explained here, and the process of writing a Card ID to a Smart Card is as follows:

 1. Connect the usb cable to the Arduino while pressing the button 1 (this will start the write mode)
 2. Start a serial monitor on the port of the Arduino with a baud rate of 115200
 3. Type the eight-digit Card ID you want to set to the Smart Card and hit enter in your keyboard
 4. Place the card on top of the PN532 NFC reader
 5. Once the Card ID is echoed through the serial monitor the card has been writen successfully 

The Card ID set in step 3 is kept in memory so users can program multiple cards in one go by placing them in on top of the PN532. If you need to write a different card just enter another eight-digit Card ID and hit enter in your keyboard.

## Duel Mode

Once you turn on the device it will start in duel mode (if you are not pressing button 1 on start). In duel mode the device is available via bluetooth to connect to the dueling host machine. It can be accessed as any other serial device via it's serial monitor. The device will output the following upon a certain action is performed:

- Button 1 press: 0\n
- Button 2 press: 1\n
- Button 3 press: 2\n
- Button 4 press: 3\n
- Button 5 press: 4\n
- Card Scanned: xxxxxxxx\n (where xxxxxxxx is the eight-digit Card ID)
- Card Scan Failure: 00000000\n

The Backend would then perform the appropriate response in the [Smart Duel Server](https://github.com/BramDC3/smart_duel_server) and show the changes on the [Smart Duel Gazer](https://github.com/BramDC3/smart_duel_gazer)

## Schematic
![Scrematic](https://github.com/AMasetti/Physical-Duel-Disk-Mid-2021/blob/main/DuelDisk_bb.png)

## Todo Do List

- Add a buzzer to make an alert while scanning the cards
- Add neopixels to reflect field behavior
- Add OLED display to show LP
