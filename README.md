<!--
//
// Copyright 2017 Luc Chouinard
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
-->

# LinenoiseBitlash
### a console for esp32 that has the scripting of bitlash and linenoise as line editing and history.

Bitlash is a tiny interpreter (C like) to which it's possible and easy to add functions to control your project. Have a look at http://bitlash.net/bitlash-users-guide.pdf for a better description. By itself, bitlash, doesn't have line editing and history, beside ^U to recall the last line. LinenoiseBitlash add a better encapsulation and line editing plus history which is done by using linenoise a tiny readline replacement. The resulting line, after pressing "enter", is executed with bitlash doCommand(). Which give, in the end, a tiny bitlash interpreter with readline equivalent line edition and history.

This library offer bitlash plugged with linenoise and a kind of encapsulation. But be advized that bitlash is C code which use a lot of global variable, most of it's code is not reentrant. And this may be the case for linenoise too.

By default a new function is added to bitlash, `termset` or `ts`, to reset the dumb terminal mode of linenoise. In the example below there is a test ( using `termProbe()` ) to probe if the terminal support escape sequences, and set the terminal to dumb mode if not. In dumb mode there is no line editing and history possible but no escape sequence displayed, which is quite annoying when there is. So `termset` permit to retest the terminal and change the dumb mode accordingly, a message is displayed if dumb mode is set, no message otherwise; `termset` is useful if you change your terminal and don't want to reset your device.

The flash partition used by EEPROMClass will be used to store bitlash script function (created using function). It you need to use EEPROMClass for other use it is possible by changing some settings in the bitlash code, it is not provided directly by this library, please read  http://bitlash.net/bitlash-users-guide.pdf at page 32, the `bitlash.h` in question is the one in `src` directory. Also when calling "ls" or "help" if you get of a lot of line with something like `function {};\n` alone on the line, it is because your EEPROM values are set to zero (which seems to be the default). You'll have to format it with `rm *`, this will initialize the flash partition used by EEPROM to 0xff. So BEWARE of `rm *`. From there `ls` and `help` will behave normally.

Pay attention when using bitlash functions like pinmode(pin,mode) pin is numeric, don't use d13 in there it won't work, use 13. And for the mode, beware because INPUT mode is not numeric 0, like in bitlash manual and arduino uno (and probably like a lot of arduino compatible boards), INPUT is numeric 1 and OUTPUT is numeric 2. To ease that aspect the textual mode names have been added as built-in in the esp32 bitlash port, they are not case sensitive, and they are as follow:

```C
	BUILT_IN("low",		"return 0")
	BUILT_IN("high",	"return 1")
	BUILT_IN("input",	"return 1")
	BUILT_IN("output",	"return 2")
	BUILT_IN("pullup",      "return 4")
	BUILT_IN("input_pullup","return 5")
	BUILT_IN("pulldown",    "return 8")
	BUILT_IN("input_pulldown","return 9")
	BUILT_IN("open_drain",  "return 16")
	BUILT_IN("output_open", "return 18")
	BUILT_IN("analog",      "return 12")
```

# INSTALL

To use this lib you will have to install:

* bitlash esp 32 port from: https://github.com/lumostor/bitlash-esp32.git
* Console library from:     https://github.com/lumostor/arduino-esp32-Console.git

These library are to be installed like normal arduino library. In my case I use platformio so I just create a link in the lib directory of my projects.

# The LinenoiseBitlash class  -- other methods provided by Console class

```C++
class LinenoiseBitlash: public Console {

 public:

  // baud: baud rate
  // p:  prompt string may contain ANSI escape sequence
  // hlen: the history length
  void begin(int baud, String p, int hlen);

  // this console task will pass the line to Bitlash doCommand() for parsing.
  virtual void consoleTask();
  
  // do the same thing as addBitlashFunction()
  void addFunction(const char * func_name, bitlash_function func) {

};
```


## Example of use

```C++
#include <LinenoiseBitlash.h>

LinenoiseBitlash CON;

numvar pin_func(void) {

  // two args this is a set
  if( getarg(0) == 2 )    ///  getarg(0) is the number of args
    digitalWrite(getarg(1),getarg(2)); // getarg(1): 1st arg, getarg(2) 2nd arg

  // always return the value read
  return digitalRead(getarg(1));
}

setup() {

	CON.begin(115200, "HAL> ", 10);

	if (CON.termProbe()) { /* zero indicates success */
          printf("\n"
	    "Your terminal application does not support escape sequences.\n"
	    "Line editing and history features are disabled.\n"
	    "On linux , try screen.\n"
	    "On Windows, try using Putty instead.\n");
          CON.termDumb(true);
  	}

	// add a new function "pin" to bitlash
	// could be use as:
	// > pin(13,0)  # set pin 13 to 0
	// > print pin(13)
	// 0
	CON.addFunction("pin",(bitlash_function)pin_func); 
	
	CON.consoleTaskStart( );  // will start a task waiting for input and execute
			          // it with Bitlash doCommand()
}

loop() {

       // put your loop code here

}


```

## Interaction example 

```
bitlash here! v2.0 (c) 2013 Bill Roy -type HELP- 118096 bytes free
HAL> ls
function blink13 {d13=!d13; snooze(100);};
function tscl {d0=!d0;};
function tsda {d4=!d4;};
HAL> help

bitlash here! v2.0 (c) 2013 Bill Roy -type HELP- 103532 bytes free
http://bitlash.net
See LICENSE for license
Pins: d0-39,a0-39  Variables: a-z, 32 bit long integers
Operators: + - * / ( ) < <= > >= == != << >> ! ^ & | ++ -- :=
Commands: arg boot else function help if ls peep print ps return rm run stop switch while
Functions:abs ar aw baud bc beep br bs bw constrain delay dr dw er ew free getkey getnum inb isstr max millis min outb pinmode printf pulsein random sign snooze
termset ts

function blink13 {d13=!d13; snooze(100);};
function tscl {d0=!d0;};
function tsda {d4=!d4;};
HAL> print free
103504
HAL>
HAL> i=0; while i<10 { print i; blink13; i++; }
0
1
2
3
4
5
6
7
8
9
HAL>

```