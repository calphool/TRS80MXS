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

February 3, 2019

Completed design of all three units.  Now cross checking them and preparing them for production at allpcb.com.
