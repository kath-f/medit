debug:
	gcc src/*.c -I include/ -g -O0 -Wall -Wextra -fsanitize=address,leak,undefined 
