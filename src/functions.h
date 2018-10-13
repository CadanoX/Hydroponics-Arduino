int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int readline(int readch, char *buffer, int len, int& pos)
{
	int rpos = -1;
	if (readch > 0)
	{
		switch (readch)
		{
			case '\n': // Ignore newlines
				break;
			case '\r': // String is finished on CR
				rpos = pos;
				pos = 0;  // Reset
				break;
			default: // Store 
				if (pos < len-1) {
					buffer[pos++] = readch;
					buffer[pos] = 0;
				}
				break;
		}
	}
	return rpos;
}