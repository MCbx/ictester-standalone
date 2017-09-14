Stand-alone IC Tester
========================

GNU GPL

This is a circuit which allows to test various ICs used in old computers. 

1. IC support list:

DRAM: 4116 (not tested), 4164, 41256, 4416, 4464, 44256, 21010, single-voltage 4116 (K565RU5 or sth like it).

74: About 70 types of 74-family and 40-family chips.

WARNING: This tester DOESN'T TEST memory on high-speed, like these GAL-based ones. It should not be used as benchmarking tool. However, extensive testing can reveal problems in e.g. failing video RAM chip in some 8-bit computers.

It consists of a few parts.
1. Tester part
2. Terminal
3. Power supply

1. Tester part consists of a 20-pin ZIF socket, ATMega328 microcontroller and 5 switches. It has also all components needed to get ATMega328 up and running (crystal oscillator, capacitors etc.). It must be ATMega328 as code is quite large (>16kB).
By default, 24MHz crystal is used, but 16MHz crystal may be used too. Baud rate of serial port is increased by 1.5 when using 24MHz. With 16MHz, baudrate is 38400, 57600 with 24MHz. Terminal works in 57600. By grounding jumper pin on terminal module you can force it to work with 38400.

2. Terminal part is connected to tester part with TTL RS232 lines and controls tester's reset line. Terminal part consists of:
 - ATMega chip (8, 328, any)
 - 2x16 LCD screen connected in 4-bit mode
 - 3 buttons (left, right, return).
 - Switch forcing tester chip reset.
The role of this part is:
 - Provide user interface for tester
 - Turn power to chip tested on and off.
 - Check power is good.

3. Power supply/converter block is responsible for generating all voltages to chip and turn it on/off.
 - Vcc for chip is generated separately from 12V (a poor-man's current limitter to avoid resetting everything)
 - 12V is regulated from input voltage, it must be at least 15V, at least 150-200mA.
 - -5V is inverted from 12V using a simple 555-based inverter and 79 linear regulator.
This is far from perfect nor efficient, but works. It also provides POWER GOOD signal to perform simple tests.

At boot-up, the tester always performs the power test. If it cannot get -5V, it doesn't allow to start and repeats tests. This is intentional because without -5V, 4116 chips tend to irreversely fail.

SWITCHES
=========
SW1,2,3 are used only with 4116 and they supply voltages for 4116.

SW4 and 5 supply ground to pins. They may be used for selecting different chip.

TESTS
=======
The device performs memory tests by writing patterns, waiting a few seconds and reading them back. It performs test in an infinite loop and the method to interrupt is to reset the tester module.
TTL logic tests are made by using test scripts encoded in program. Tester performs 100 repetitions of test, if any fails, it fails the test, if all passes, it displays that chip is OK. You can add your own chips by modifying scripts array, but pay attention to Vcc and GND. 
GND in DIP14 logic chips is connected a very bad way (40mA efficiency). This will likely work, but in some rare and antique chips (especially Soviet K155 from 1970s) tests may fail.

Supplied, there is a mod2script.pl program which converts ICTester's MOD files directly to series of values used as scripts in program.

Exact description of script language used by tester is inside comments of source files.


MCbx 2017
http://oldcomputer.info
