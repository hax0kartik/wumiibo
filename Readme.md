# Wumiibo
Amiibo Emulation for 3ds over network.

## How To Use
 * Put the `0004013020004002` folder in `luma/titles/`
 
 * Enable Title patching from luma menu
 
 * Obtain the [client/companion](https://github.com/hax0kartik/wumiibo-client/) app
 
 * Open your game and reach to the screen where it tells you to place your amiibo.
 
 * Open the companion app, select your encrypted and decrypted amiibo files, Enter your 3ds's IP and click on `Emulate`.
 
 __NOTE: Your 3ds and your device should be connected to the same network__

 * If everything went successfully, your amiibo should be emulated now.

 __NOTE: Some Games can get stuck on "Remove the amiibo to continue" screen. Press the `START` button and wait for 5-6 secs. If it doesn't work, open a github issue__


## Reporting bugs

All cmds are currently not implemented, so expect bugs.
Please use github issues to report any such games which don't work.

## How to Compile
Get devkitpro, ctrulib and makerom and then `make -j` to compile.

## Technical Details
This is a rewrite of the 3ds's nfc module to enable amiibo emulation. The main IPC handler is located in the client/companion itself. All it does is relay the cmdbuf to the companion and then pass the information to the games. It does have a "dummy" IPC handler which handles some critical funcs. No parsing of data is done here, the client is responsible for that.

## Credits
@Luigoalma for advising me and for listening to my rants.

@HiddenRambings for `noamii`, which served as a base for me to extend and develop upon.

@3dbrew folks for the nicely documented nfc and amiibo pages.

@Socram8888 for amiitool, reversing nfc, etc.

And surely many more people out there, whom I have forgotten to mention.