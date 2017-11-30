
#include <LinenoiseBitlash.h>

// this function added to bitlash permit to adapt linenoise dumb mode.
// With terminal that doesn't support escape sequences, the dumb mode remove escapes
// sequences from the stream but remove also line editing and history "browsing", the
// line is still put in history. If you got into one of the two modes and want to
// reset it just type 'termset' in bitlash, it will adapt to the terminal.
numvar termset_func(void) {

  int probe_status = linenoiseProbe();
  if (probe_status) { /* zero indicates success */
    printf("\n"
	   "Your terminal application does not support escape sequences.\n"
	   "Line editing and history features are disabled.\n"
	   "On Windows, try using Putty instead.\n");
    linenoiseSetDumbMode(1);
  } else {
     linenoiseSetDumbMode(0);
  }
  return (numvar)probe_status;
}

