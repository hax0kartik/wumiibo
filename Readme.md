# Wumiibo
Amiibo Emulation for 3ds.

## How To Use
 * Put the `0004013000004002` folder in `luma/titles/`
 
 * Download the amiibo you want to emulate from [here](https://hax0kartik.github.io/amiibo-generator). All this website does is produce a bin file with amiiboID in it, so the produced file can be shared freely!

 * Put the downloaded bins in `sd:/wumiibo` (Max 50)

 * Enable Title patching from luma menu
 
 * Open your game and reach to the screen where it tells you to place your amiibo.
 
 * Press L + DOWN + START to bring up Wumiibo Menu and select the amiibo you wish to emulate.
 
 * If everything went successfully, your amiibo should be emulated now.

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

thomas, Marcus777, k9999, Mr.Faq, MissingNerd and many others who helped testing wumiibo

And surely many more people out there, whom I have forgotten to mention.