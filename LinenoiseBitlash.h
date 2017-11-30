#ifndef LINENOISEBITLASH_H
#define LINENOISEBITLASH_H

//  -*- c++ -*-
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


#include <Console.h>
#include <bitlash.h>

extern void addBitlashFunction(const char *name, bitlash_function func_ptr);
numvar termset_func(void);


class LinenoiseBitlash: public Console {
 public:
  
 // LinenoiseBitlash(): Console() {
 //   
 //  }

  void begin(int baud, String p, int hlen) {

    Console::begin( baud, p, hlen) ;
    initBitlash(baud);
    addFunction("termset", (bitlash_function)termset_func);
    addFunction("ts", (bitlash_function)termset_func);
  }

  // will be executed by Console::consoleTaskStart()
  // 'virtual' is important for that to be
  virtual void consoleTask() {
    char *line;
    for(;;){
      if((line = lineNoise()) != NULL) {
	historyAdd(line);
	//printf("LinenoiseBitlash! You wrote: %s\n",line);
	doCommand(line);
	freeLine(line);
      } 
    }
  }    

  void addFunction(const char * func_name, bitlash_function func) {

    addBitlashFunction(func_name, func);
  }
};



#endif  //#ifndef LINENOISEBITLASH_H
