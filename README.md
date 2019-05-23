# TRS80MXS
TRS-80 Modular eXpansion System

![Fully Stacked TRS80MXS](img/TRS80MXS_Stack.jpg?raw=true "TRS80MXS")

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

April 21, 2019

I reworked the base board design and the graphics board design.  I'd added a switching supply for the 5V, but I left the 7805 in 
there as well, with a jumper to allow you to switch back and forth.  Since I don't know if the switching supply will be too noisy,
I figured I wanted to have something that I knew would work.  The reason I had to redesign the base power board was that I had 
some fuzzy thinking going on.  I had two 7805 regulators on the original board, with their outputs tied together.  For some reason
I was thinking that I could manage more current that way, but that's only true in a really crude and kind of useless way.  Basically
that configuration is a known antipattern.  What happens is the two 7805s sort of "fight" to see who's going to provide the 5V, one
wins, takes all the current, overheats, and then the other suddenly jumps in if its sibling's heat overload kicks in.  That's not 
really useable.  So instead, I figured I'd switch to a higher amperage switching circuit built around an LM2596.  Since I don't know
if the noise will be too much, I left one of the 7805s in place, and made it so you could use a jumper to switch between the two.  

All of that got me to thinking that maybe having a "regulator board" wasn't such a dandy idea anyway.  It forces all the current 
to be fed from that one board, which in turn creates a heat problem if I use linear regulators.  So, I also ran the raw 9V in to 
one of the previously unused header pins, which means that in future designs the shields could regulate their own power if need be.
I probably should swap out all the 74LS* chips for 74HCT* chips to reduce power consumption.  Save the planet man.  What are you 
doing with all these old power hungry chips?  (My experience has been that 74LS* chips are rock solid at the design stage, and then 
you can pivot to HCT or ACT later to get additional desirable qualities -- you just have to sort of "ease" them into your design 
to make sure you don't get caught with some unexpected behavior difference -- sometimes related to timing.)

Additionally, I added some inductors to the graphics board.  These are on the TI-99/4A schematics for the VDP, and presumably they're
there to stabilize power.  I noticed that the TI-99/4A circuits have several of these chokes throughout, and there's a clean 
separation between normal VCC and VCC for video, with the chokes being the main difference.  So far I haven't needed these, but 
I figure the TI engineers probably had them there for some reason.

I've been working on some audio software.  Soon I think I'll have a 12-part harmony audio player.  It's sort of like a MOD tracker
from back in the day, but it doesn't play samples -- just manipulates the tone generators on the sound chips.  I'm anxious to 
get it working fully so I can try out some stuff.  I'll probably build some kind of midi converter as well.  Interestingly 
enough I haven't yet been able to find a midi file that tries to use 12 voices at once.  I wrote a little utility that calculates 
tone depth across all midi channels, ran hundreds of midi files through it, and so far I've only been able to find one song with 
more than 10 simultaneous voices, so 12 tonal voices might have been overkill.  Oh well.  ;-)

April 13, 2019

Got the third board working.  Getting the GAL22V10D programmed ended up being a pain.  I don't know why, but my VS4800 programmer just decided 
it wasn't going to program my ATF22V10C chips, so I bought some GAL22V10D chips.  It wouldn't program them either.  So, I decided to install VSpeed
(the programmer software) on another machine, and although at first it did the same thing, after fooling around over and over again (using the "all compare" function seemed to somehow cause it to start working).  One thing I can say for sure using that software and programmer is that until you issue the program instruction followed by a Read IC instruction and the checksum matches, you haven't programmed it properly.  You can also avoid the encrypt function, because it introduces its own chaos into the mix.  I suppose if I had an expensive programmer I wouldn't have to deal with that nonsense.

So, today is basically the culmination of a lot of hard work for a long time.  Today I should be able to write code on my Mac, create a disk image with trstools, sneakernet that code over a microSD card to my floppy emulator or upload it via USB to the teensy, and now I've got 12 channel audio and graphics (and of course 32k of extra ram on the power distribution board).  I'll go through a few cycles of that in the next few days to prove that it works, maybe create a build pipeline for my pacman game.  

I'll decide after I get a few rounds of that under my belt if I want to design/build one more shield board -- an ESP32 board.  This would give the system 
one additional feature that could be pretty useful -- wireless connectivity.  For my build pipeline that would make it so I could skip the sneakernet 
step, do a compile on the computer and transfer wirelessly (maybe -- I would have to figure out how to get the data over to the teensy.  Maybe some transfer program that would move data from the ESP32 to the Teensy.)


March 17, 2019

Happy St. Patrick's Day!  Got the redesigned drive emulator "shield" working.  Last board is the video/sound board.  There's some LED weirdness going on with
the drive emulator board (probably pin reassignments or something), but I'm not going to fool around with that right now.  The drive emulation works as 
expected, so that's good enough.  Stay tuned for the final board.  It has the most dramatic changes, so it may take a little more debugging to get fully 
working.  Once it's done, then our layer sandwich is basically done, except for the wireless board that I haven't yet designed. 

In totally unrelated news, I bought an Apple IIGS for $250.  I don't have boot disks, so it tries to boot and then complains that it has nothing to 
boot from.  I've ordered some disks from ebay.  Gotta love the IIGS.  It was Woz's last serious involvement in engineering at Apple, and some argue that it 
saved the company because it extended the aging Apple II line just long enough for Apple's board to realize that Jobs didn't know what he was talking about 
trying to push the Mac line when the technology just wasn't there yet.  Some people slather a lot of praise on Jobs, and certainly he was a visionary, but
his visions were often too far ahead of what the technology could do at the time, which put the company at risk in the late 80s.  Vision is great and 
necessary, but engineering is where the rubber meets the road.  Any science fiction writer can imagine amazing technology.  Making it actually happen 
requires hard skills.

March 3, 2019

I got the first instance of the base board built and tested.  It had a bug in the RD*, WR*, IN*, and OUT* signal lines.  I just started using 
[busses in EagleCad](./img/bus.gif?raw=true), and the user interface is incredibly awkward.  It's *super* easy to accidentally combine bus lines in weird ways.  In my case 
I had combined the buffered and unbuffered copies of the RD*, IN*, OUT*, and WR* lines, and it was creating havoc.  Even after removing the 74LS244
that was supposed to be doing the buffering, it was still screwing stuff up.  So I had to cut several traces around the chip and wire it properly.
I updated the design in EagleCad (it's now version 1.1), but didn't send for new boards at this point.  If someone wants to buy a few of these things 
or something I'll go ahead and order some more.  [Here's a pic of the completed board](./img/TRS80MXS_Base_v1.0.jpg?raw=true)... well, mostly.  I didn't have a 3.3v regulator handy, so I left that off for now, easy enough to solder in later when needed.  I also didn't have 4.7k resistor networks of the right size, so I cobbled together a hand full of discrete resistors and made an array out of them.  It's not pretty, but it's functional for now.  I also modified a switch to be a right angle mounted switch, which is a little kludgy but it's stable and will be hidden in plastic at some point anyway.  So yeah, not beautiful, but completely functional and I'll be able to build the other two "shield" modules and snap them on there.  Next I'll get the floppy emulator moved into this new form factor and tested.

Oh yeah, talking to my friend about the project, he expressed that he'd like to be able to browse the web with his TRS-80.  That's a bit o a tall order, 
but in theory would be possible with one more "shield".  Namely, an ESP32-based shield that was interfaced in the same way that the floppy emulator is 
interfaced (with a memory location peep hole mapping via a flip-flop and some latches, along with a WAIT* pin to aid in timing.)  So yeah, I may do that 
as well.  That'd make for quite the [sandwich](./img/sandwich.jpg?raw=true) of boards (base + graphics/sound + ESP32 wifi + floppy if we want to use it more than once), but in principle it would work.  The hardest part would be the browser rendering engine itself.  It might be one of those projects where
you can get the skeleton of the thing working fairly easily (basic text handling like a Lynx or maybe Browsh browser), and then graphics and sound would require a lot of design, which can turn into someone else's labor of love if they want.

February 22, 2019

I received my PCBs the other day.  Unfortunately I didn't do a great job of carefully reviewing them before sending them to the fab house.  There are 
so many little things to check when you're making a PCB that it's easy to miss something.  In this case, I didn't double (and triple) check the 
orientation of the headers on the boards.  I had them oriented correctly on the base and the floppy emulator PCB, but I didn't have them right on the 
sound/graphics board.  So, I had to flip them (both horizontally and vertically... wow was I off!), and then move everything around on the board 
because all the traces were now too far away from their respective chips.  *sigh*.  Oh well, I'll remember next time.  So now that's done and I've 
got the graphics/sound board headed back to the fab house.  I wish I could get some kind of deal on the shipping.  The cost of the fabrication is 
nothing ($5 for five boards), but the shipping is $20.  I can start populating the base and floppy PCBs now, so I'll probably do that this weekend.  One
of the cool things about these boards is that by making them modular like this, I think I should be able to use them on other 8-bit computers... basically
the base board will be unique to each computer, but the sound/graphics and floppy emulator may be pretty close to just working as a "cartridge" in 
other 8-bit PCBs of the era.  Of course, they'll need different software, but I don't think it'll be a total overhaul to get them working with say 
an Apple II, Commodore, or Atari 8-bit.  We'll see.  I mean, there will be differences because those computers were built around a 6502 and these 
cards were built for a Z80 machine, but a lot still translates -- an address bus, an 8-bit data bus, and so on.  We'll see when we get there.  If I 
keep plugging away I'm sure we can figure out a way to create a set of semi-universal cards... though maybe that's more of a long term goal.

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
