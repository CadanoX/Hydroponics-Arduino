int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

/*
We are reading inputs from the Serial channel into the buffer.
Commands come in char by char, which is why readline is continuously
called until CR ends the command.
The function returns -1 unless the command is complete.
When CR completes the command, the number of read chars is returned
and the buffer position will be reset to await the next command.
readch is an input character on the Serial channel
The input is read into a buffer of size len
pos keeps track of the current buffer position to write to.
*/
int readline(int readch, char *buffer, int len, int &pos)
{
  // Index of the character in the read channel
  int rpos = -1;
  // Check if there is an input on the read channel
  if (readch > 0)
  {
    // Filter special characters
    switch (readch)
    {
    case '\n': // Ignore newlines
      break;
    case '\r':    // String is finished on CR
      rpos = pos; // Return length of input command.
      pos = 0;    // Reset
      break;
    default: // Copy input char into buffer
      // Prevent overflow and keep one char for CR
      if (pos < len - 1)
      {
        buffer[pos++] = readch;
        buffer[pos] = 0;
      }
      break;
    }
  }
  return rpos;
}