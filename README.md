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

# Additional characters from existing ITF file

If for some reason there is a need for very special characters that are not in a ttf file but inside a itf instead,

you can add the following line to the config_font.ini:

ITFReferenceFile=font_enx.itf

This will tell the tool to pick the characters inside this font when they are not present in the provided TTF file.

However, there is a problem of alignment. To my understanding, there is no way to know the base of the character from 

the itf font data. Therefore you will need to align those special characters manually inside the resulting itf file.

To do that, first look for the character inside the itf file by looking for its UTF32 code in the first section. 

Next to its code is the address inside the file. When going to this address (addr), you can adjust the position of the character 

by editing the value at addr + 4 (for y axis) and addr + 6 (for x axis).

Let me know (feitaishi45@gmail.com) if you encounter a problem or have a suggestion regarding this matter.