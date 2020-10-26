# Wumiibo 
<img src="wumiibohelper/gfx/icon.png"/>

![License](https://img.shields.io/badge/License-GPLv3-blue.svg)
[![GitHub release](https://img.shields.io/github/release/hax0kartik/wumiibo.svg)](https://github.com/hax0kartik/wumiibo/releases/latest/)
[![Github all releases](https://img.shields.io/github/downloads/hax0kartik/wumiibo/total.svg)](https://GitHub.com/hax0kartik/wumiibo/releases/latest/)

Amiibo Emulation for 3ds.

## How To Use
You need to have the latest luma for this to work correctly.

 * Put the `0004013000004002` folder in `luma/titles/`
 
 * Download the amiibo you want to emulate from [here](https://hax0kartik.github.io/amiibo-generator). All this website does is produce a bin file with amiiboID in it, so the produced file can be shared freely!

 * Put the downloaded bins in a folder and put that folder in `sd:/wumiibo`. You can also place the bins directly inside `sd:/wumiibo`. You can have upto 49 folders and inside every folder you can have more subfolders. So if you sort your bins correctly, there is no limit on how many amiibos you can have at a time.

 * Enable Title patching from luma menu. (As shown here: https://github.com/LumaTeam/Luma3DS/wiki/Optional-features)
 
 * Open your game and reach to the screen where it tells you to place your amiibo.
 
 * Press L + DOWN + START to bring up Wumiibo Menu and select the amiibo you wish to emulate.
 
 * If everything went successfully, your amiibo should be emulated now.

If you want to use your real amiibos, you'll need to disable wumiibo by disabling game patching from luma menu.

## Workaround for games which freeze

Some games freeze after closing wumiibo menu, check the [compatibility list](https://github.com/hax0kartik/wumiibo/wiki/Compatibility-List) for these games. The following workaround can be used to emulate amiibos in such games.

 * After you have opened your game, press home button. 

 * Open wumiibo menu and choose what you want to do.

 * Open the game again.

Do not try to open wumiibo menu in such games.

## Reporting bugs

Please use github issues to report any games which don't work.

## How to Compile
Get devkitpro, ctrulib and makerom and then `make -j` to compile.

## Technical Details
This is a rewrite of the 3ds's nfc module to enable amiibo emulation.

## Credits
@Luigoalma for advising me and for listening to my rants.

@HiddenRambings for `noamii`, which served as a base for me to extend and develop upon.

@3dbrew folks for the nicely documented nfc and amiibo pages.

@Socram8888 for amiitool, reversing nfc, etc.

@Luma3ds devs lots of code has been copied from rosalina

druivensap, loveleiz, thomas, Marcus777, k9999, Mr.Faq, MissingNerd and many others who helped testing wumiibo.

And surely many more people out there, whom I have forgotten to mention.

----

If you would like to support me you can buy me a coffee on [ko-fi](https://ko-fi.com/hax0kartik) or you can [paypal](https://www.paypal.com/paypalme/preetiagarwala?locale.x=en_GB) me! Thank you!