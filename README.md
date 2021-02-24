# FalcomFontCreator
A simple tool to generate a .itf font file from a .ttf (TrueType font) file. Those fonts are used in recent Falcom games such as Trails of Cold Steel or Tokyo Xanadu.
Please don't take this tool "too" seriously, I didn't spend that much time studying it, I can't explain everything in the font file, but I have a general understanding of the file structure and thus the output file should work.

# How to use
Just fill the "config_font.ini" file then run the Font Creator.

Resolution=64 <= this is the resolution of the font, 64 should be "HD" enough

FontSize=512 <= the font size

Font=cuprum.ttf <= the path to the input TTF file

Base=26 <= something to play with to realign all the characters if a big one is misaligned (putting a greater value should help fixing the font)

NbChar=4095 <= 4095 means we will try to render the first 4095 characters in UTF32 using the TTF file (the higher the number, the heavier the itf file gets)

