# TRS80MXS
TRS-80 Modular eXpansion System

![Fully Stacked TRS80MXS](img/TRS80MXS_Stack.jpg?raw=true "TRS80MXS")

This is repo is for the TRS-80 Modular eXpansion System (MXS).  The intent is to make it the final of a series of 
prototypes (TRS80HDD and TRS80GS and a memory expansion module).  

The MXS consists of four modules set up as a stackable set.  So, you can build the base module first, which will 
give you a 32k memory expansion for your TRS-80 Model 1, as well as a power conditioning base that the other boards
use.  Then you can make either the floppy drive emulator, the TI video/graphics board, or the WIFI board (or all three).  
The boards snap together.  

Theory of Operation
===================

The basic theory of operation of the MXS is covered in the other projects in this repo.  The combined unit has a few 
unique characteristics.  First, it has three headers at the north, and south positions, and northeast of each board.  These 
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

September 21, 2020

I've been fooling around with a few more designs recently.  Someone asked me if I had considered building a stand alone joystick board.
I had never considered it, but I went ahead and put two together.  One supports two joystick ports, and allows you to select the port 
numbers with dip switches.  The other is bare bones.  It only supports one joystick, but it can be built for somewhere between $15 and $30.

I also stumbled on a weird bug in the Teensy code for the floppy emulator.  When you swapped the disk image, every once in a while the 
TRS-80 would either lock up or show random garbage on the screen.  It looks like it's some kind of weird timing bug, most likely because 
the SD drive emulation is a little slow, and the code was leaving the WAIT signal low while the entire mount command was active.  So I 
tweaked a couple of things to fix that.

Also, just a quick note.  Jan Beta did a [Septandy video](https://www.youtube.com/watch?v=hyTF-FG4Bv8) where he's working on a Model 1.  He
didn't have a power supply for it, so he built one.  As I was watching the video, he briefly pulls out the schematic of the power supply, and 
I was like "Hey, that's *MY* power supply design!"  Unfortunately, he ends up losing a bunch of time in his debugging exercise because he 
didn't have one of the center tap lines connected correctly.  Incidentally, the way he constructed his power supply looks a lot more 
professional than mine.  I just threw mine in an electrical gang box, but his looks like it has a nice case and everything.


June 28, 2020

I updated the graphics/sound board again.  This time I made it so that the joystick ports are compatible with the "Alphastick" joystick
that was sold back in the early 1980s.  Several games support this joystick.  Also, I added a pair of jumpers that switches between the two 
different kinds of joystick trigger modes supported on these old joystick designs.  

I also learned how to use graphics mode 2 to produce near-bitmap style graphics.  So, I created a start-up screen for the MXS board that runs 
as soon as NewDos/80 loads up.

I finished the Pac-Man clone.  Now I've started on a Joust clone.  Joust runs in graphics mode 2, which makes for amazingly accurate reproduction of the 
background.  I've been working on the SDL emulation layer so that I can duplicate the TMS9918's "only 4 sprites per scanline" behavior.  I need to
duplicate this behavior so that I can come up with a "flicker routine."  This was the approach used for TMS9918 games back in the day.  You 
detect the situation, and then rotate the sprites' priorities in order to make sure that no one sprite turns off completely.

So I'm at version 1.4 of the graphics/sound board.  It's done and working well.  I have given some thought to removing two of the sound chips 
(do I really need 12 voices?  6 seems like it ought to be adequate), and replacing them with a TMS5220 speech synthesis chip.  This would make the 
board quite versatile.  Though the TMS5220 has become difficult to find, so I'm not sure.  I'll probably finish the Joust clone first.


May 9, 2020

Well, with the pandemic I've had a little time to work on the project some more.  I haven't worked on the wifi board recently,
but I decided to put together at least one piece of software that demonstrates the graphics/sound/gaming capabilities of the 
TRS80MXS.  [Here's a little video of the Pac-Man clone.](https://youtu.be/DzdaCiHI2xc)  It's a work in progress, but it's at least starting to look like a playable game.


January 1, 2020

Well, we're in the new house, and semi-settled.  Always a million things to do when you move to a new house.  
However, I've got a little time to begin working on the Wifi module again.  As luck would have it, I'm now living 
only a few blocks from my friend who started this little adventure.  Just for fun over Christmas, I had my younger 
daughter come play some of the games that are loaded on the HDD module.  I even hooked up the cassette port to 
an amplifier, and I connected the composite video out on my old TRS-80 to our big screen TV.  It's kind of 
amazing when you think about it -- that computer is 42 years old, and I can connect it to a brand new flat panel display.
My daughter laughed her head off at these "cheesy old games."  She said something like "Dad, how could you guys 
stand to play these silly things?  There's no color. They're like really bad rip offs of arcade games that weren't 
that great to begin with!"  The answer of course is pretty simple.  We had no choice, and we *thought* they were 
pretty cool games.  In fact, I'm still pretty amazed when I ran a couple of those games and the authors had managed 
to implement a sample playing routine into the game so that when it started it said something like "Ready Player One" 
and "Game Over".  I mean, seriously -- in 1977 somebody had figured out how to sample audio and misuse the audio 
cassette port to drive voice samples.  It may not be impressive to my 20 something daughter, but it's still pretty 
impressive to me.  Perspective I guess.

So I'll see if I can wrap up the Wifi module in the next few weeks.  It was intended to be a general purpose 
wifi module, but I'm running into some things that make me think that may not be practical / easy.  The main one is 
that the ESP32 designers didn't really imagine that someone would use their module for a general purpose HTTP client.
The issue is basically RAM constraints.  We take it for granted these days, but TLS security requires a *lot* of 
RAM to store a bunch of root certificates.  The code that implements the TLS stuff for my ESP32 module makes a 
rather huge assumption that you're going to have a single site that the module is going to talk to, so you don't
really even try to load a bunch of root certs into it.  You load one for the site your're going to be accessing.
That obviously doesn't work very well when you're trying to build a general purpose tool.  Now I *could* probably
implement a proxy in AWS or something, and then set up my ESP32 to work against that HTTP proxy, but I don't really 
want this hardware to be a slave to some stupid proxy running in AWS.  So, I'm at a bit of an impass.  I *may* be 
able to figure something out, but it'll require me to dig deeply into the TLS libraries for my ESP32, and basically 
rewrite a huge chunk of their code.... and who knows what the performance will be like.  The other possible route 
would be to do something like reduce the functionality of the Wifi module so that it just gives you access to a 
particular hosting site, like maybe DropBox, but that's a bit of a let down.  We'll see what I can come up with 
over the next few weeks.

October 21, 2019

I'm still working on the project.  Things have slowed down a bit over the past few months because my eldest daughter got married, 
my youngest daughter started college, and I'm selling my house.  Once I get settled in the new house I'll pick up the 
Wifi module again, which is the last one to get completely working.  It "sort of" works right now, but it's built around the 
ESP32 module and that thing is sort of a bugger to code for.

July 25, 2019

I purchased a TRS-80 Model 1 on Ebay, so I don't have to borrow my friend's any more.  Unfortunately, it didn't come with a 
power supply.  So, I designed one.  Maybe someone with a bad power supply can use my design.  Here's a pic of the [schematic.](img/powersupply.pdf?raw=true)
The schematic and board layout is in the eaglecad folder.  I didn't actually use the board layout.  I just hand wired everything for my supply, and put it 
inside an electrical utility box.  If you decide to make one of these, be very careful.  You'd working with 110V mains voltages, so you need to know what 
you're doing.  I'm not responsible if you electrocute yourself!)

July 7, 2019

Figured something out that's worth documenting.  For a long time I've thought that something was wrong with drive 3.  I was 
using NEWDOS/80 2.0 disk images in drive 0 (boot drive).  Apparently there's a command (PDRIVE) that changes how DOS handles
different drives.  The drive image I was using had the wrong settings for drive 3, and by dumb luck and coincidence had 
usable settings for the other 3 drives.  As soon as I changed drive 3's TI and TD settings, it started working properly, 
so there's nothing actually wrong with drive 3!

July 4, 2019

I just designed and uploaded the WIFI module.  It hasn't been built yet, and no software has been written, but the design is 
there.  I'll get the board and parts on order and see what we get.  Chances are it won't work right off the bat, but you 
never know.  I decided to design this board with a couple of nuances.  First, it is built around port IO rather than memory 
mapping.  Since the port numbers aren't really standardized on the TRS-80, I decided to make the address decoder selectable 
with 8 DIP switches, so any port between 0 and 255 can be chosen.  The other nuance is that I didn't use any GALs or CPLDs for 
the glue logic.  That should make this board a little easier to build than the graphics/sound board.

June 28, 2019

I wrote a utility (called "trs80mxs") that runs on the TRS-80 to manipulate the drive images that are stored on the MicroSD card.  Before this utility,
you had to do your manipulations from a Mac connected via USB.  Since that forced the MXS to be tethered, that didn't seem like a good long term solution.
So, I "stole" the address the TRS-80 uses for printers and turned it into a "general purpose communication buffer" between the Teensy and the TRS-80.  So,
the Teensy receives data, turns it into SD Card manipulation commands, and replies back with what happened.  So the trs80mxs command utility doesn't 
really do much -- it just sends commands to the printer address, and reads the response back through the same address, and prints it to the screen on 
the TRS-80.  With it you can list disk images that are on the SD card, mount them onto one of the 4 emulated drive slots, and list what is currently 
mounted.  I've also created a video of using the utility [here.](https://www.youtube.com/watch?v=PgaojUgeEG4)

June 16, 2019

I tweaked the Teensy code to support deleting and writing files now.  So now drives 1-3 work as perfect emulators for JV1 formatted files.  I still have no
idea why drive 4 acts flaky.  The code is literally identical.  It's got to be some bug in the Model 1 hardware behavior when you've got 4 floppies. I also
uploaded some more audio tools.  midiDump accepts a midi file and converts it to a large array that can be copy/pasted into one of the player files like 
bach.c and then compiled with z88dk to create a .cmd file.  It's a tiny bit buggy, but it more-or-less produces the right data.  One of these days I'll 
dig into what precisely is causing it to act a little flakey.

June 10, 2019

Improved the timing just a bit.  Hear the difference [here.](https://www.youtube.com/watch?v=ByLgBegGoTc)

May 31, 2019

I just uploaded a demo program that plays a J.S. Bach tune - Invention 13.  [Here's a video of it playing.](https://www.youtube.com/watch?v=8Pr8rdETvlA)  The player isn't 
quite perfect (some slight timing issues), but it's certainly recognizable.  I used a preprocessor that's located in the OSX code that reads a midi file and turns it into 
an array of structures.  This isn't quite ideal.  It would probably be better to merge the two so that the TRS-80 can directly play midi files.  I may go ahead and piece 
that together over the weekend if I can get the midifile cpp code to compile on z88dk (the compiler I'm using for the TRS-80).

May 27, 2019

I got some of the sound code working.  It took a while to figure out what was going on in playnote.c.  One of the big pains in 
the sound chips is that I wired the data lines backward due to strange labelling in the sound chip data sheets.  This requires 
you to think of the bits backward.  Anyway... got it working.  It's pretty cool hearing 12 simultaneous harmonious voices.

May 23, 2019

I am still using the previous version of the PCBs at this time.  I've removed one of the 7805 linear regulators and 
replaced it with a TRACO TSR2-2450 switching regulator.  It appears that the regulator is stable enough, which was one of 
the questions I had about those regulators.  It appears that they'll work okay.  I'll probably spend some time this weekend
working on software.  I'd like to get an audio demo up and working, then some video, and then I'll pivot to working on a 
wifi shield at some point in the near future.


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
