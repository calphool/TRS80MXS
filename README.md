# TRS80MXS
TRS-80 Modular eXpansion System


This is repo is for the TRS-80 Modular eXpansion System (MXS).  The intent is to make it the final of a series of 
prototypes (TRS80HDD and TRS80GS and a memory expansion module).  

The MXS consists of three modules set up as a stackable set.  So, you can build the base module first, which will 
give you a 32k memory expansion for your TRS-80 Model 1, as well as a power conditioning base that the other boards
use.  Then you can make either the floppy drive emulator or the TI video/graphics board (or both).  The boards snap together.  

Theory of Operation
===================

The basic theory of operation of the MXS is covered in the other projects in this repo.  The combined unit has a few 
unique characteristics.  First, it has three headers at the north, and south positions, and norteast of each board.  These 
are what allow the boards to be stacked.  One header is mostly the address bus of the TRS-80 expansion connector, 
passed through a 74LS244 buffer.  It also includes the operation signals (RD, WR, IN, OUT).  The opposite header 
contains the data bus and a few miscellaneous signals.  Finally the header at the northeast position is 4 lines that 
are OR-ed into the WAIT* signal as well as another 4 that are tied into the INTERUPT* signal.  This allows the 
MXS to host 4 separate chips that might use the WAIT* mechanism (as the sound chips and the floppy emulator 
flip flop do), and 4 interrupt chips (which presently are only used by the floppy emulator for its clock and 
drive activity interrupts).  The floppy emulator board also includes an 8x2 header that consolidates all the 
unused Teensy signals as well as a ground, 5V, and 3.3V signal.  These pins can be utilized to extend the functionality
of the Teensy unit.


Build Notes
===========

February 9. 2019

Found [this interesting diagram.](https://www.kguttag.com/wp-content/uploads/2017/11/198x-External-Video-by-KG.pdf)  It may be worth digging into this later.  It would be kind of cool to be able to merge the TRS-80 
video signal with the TMS9918 signal such that the TRS-80 signal is the back plane.

February 8, 2019

I decided to add two more sound chips to the graphics/sound shield.  So now there should be a grand total of 12 tonal voices
and 4 noise / drum voices.  That will make the sound card significantly more capable than it was before.  I also decided 
to add a divide-by-four circuit to the second set of sound chips' clock.  So, they'll play at exactly 1/4 the pitch of the other 
two sound chips, making the bottom attainable frequency something like 27hz, which is around the bottom A of a piano keyboard, 
thereby fixing one of the terminal annoyances of the SN76489, its ridiculously large range that unfortunately tends to 
start in the middle of the keyboard with the standard clock frequency.

In order to add the extra sound chips, some surgery was necessary on all three boards.  I needed to change the base board 
to accommodate more wait signals, which in turn meant that I needed to add an 8-input NAND, which in turn meant that the
4-input NANDs I was using before were sort of redundant.  So now there are 8 available interrupt lines and 8 wait lines, in a 
4x4 pin grid at the north east side of the board.  Five wait signals are being used now (4 for sound and one for the 
floppy drive).  One interrupt is being used by the floppy emulator, and I've made the video interrupt optional with a jumper.
I don't think that interrupt will ever be especially useful (since the floppy drive interrupt occurs 25 times a second 
anyway, we might as well just poll the video card for anything we need), so making it optional seemed smart.

In order to add the extra sound chips I had to dispose of the ability to issue a VDP reset from software (needed the pins 
on the CPLD).  Also, I had to reorder a couple of the pins on the CPLD because for some reason one of the pins couldn't handle the 
complexity of the logic equation I was feeding it.  (Wouldn't compile in WinCupl.)

I'm thinking maybe at some point I'll throw together a MIDI player for this sound card.  With 12 voices and a full keyboard range and 
then some, I should be able to play practically anything.  Who knows, maybe with a little clever programming I'll be able to get some 
different timbres and be able to handle MIDI program change messages even.  12-part audio is better than some polyphonic synthesizers, 
so this should be interesting.  The only concern I have is the amount of data I'd be jamming down the bus to accomplish anything interesting.
Each tone change is two OUT instructions, so to play 12 voices at the same time you'd be executing 24 instructions.  I'm not sure
if the time gap between the first note and the last will be audible or not.  I hope not.

So, now I think I'm about ready to send these things to a fab house.  


February 3, 2019

Completed design of all three units.  Now cross checking them and preparing them for production at allpcb.com.
