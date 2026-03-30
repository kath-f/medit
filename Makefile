CFLAGS=-Wall -std=c99
DEBUG_FLAGS=-Wextra -Wuninitialized -Wmaybe-uninitialized \
	    -fsanitize=undefined -fsanitize=address \
	    -g -O0 \
	    -DME_DEFAULT_LOGLEVEL=ME_LOGLEVEL_DEBUG 
SRC=src/*.c
INC=include/
OUT=medit

default:
	gcc $(SRC) -o $(OUT) -I $(INC) $(CFLAGS) -DME_DEFAULT_LOGLEVEL=ME_LOGLEVEL_INFO
debug:
	gcc $(SRC) -o $(OUT) -I $(INC) $(CFLAGS) $(DEBUG_FLAGS)
combine:
	cat include/*.h src/*.c > Medit_Combined.txt
