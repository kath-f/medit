/* ME_Print.c
   Implementation file for ME_Print.h
 */

#include "ME_Print.h"

#include <stdarg.h>
#include <stdio.h>

ME_LogLevel ME_GLOBAL_LOGLEVEL = ME_DEFAULT_LOGLEVEL;

void me_print(ME_LogLevel loglvl, const char* str, ...){
  if(loglvl <= ME_GLOBAL_LOGLEVEL && ME_GLOBAL_LOGLEVEL != ME_LOGLEVEL_NONE){

    //if loglevel is info print to stdout, otherwise print to stderr
    FILE* out_buff = (loglvl == ME_LOGLEVEL_INFO) ? stdout : stderr;
    
    switch(loglvl){
    case ME_LOGLEVEL_ERROR:
      fprintf(out_buff, "\e[91m[ERROR] \e[0m");
      break;
    case ME_LOGLEVEL_WARN:
      fprintf(out_buff, "\e[95m[WARNING] \e[0m");
      break;
    case ME_LOGLEVEL_INFO:
      fprintf(out_buff, "\e[0m");
      break;
    case ME_LOGLEVEL_DEBUG:
      fprintf(out_buff, "\e[94m[DEBUG] \e[0m");
      break;
    case ME_LOGLEVEL_NONE: //This case has no chance of happening, but i put it here because
      return;              //if i don't gcc will yell at me
    }

    va_list args;
    for(va_start(args, str); *str != '\0'; str++){
      if(*str == '%'){
	str++;
	switch(*str){
	case 'c': //char
	  fprintf(out_buff, "%c", va_arg(args, int));
	  break;
	case 'i': //sint
	  fprintf(out_buff, "%i", va_arg(args, int));
	  break;
	case 'u': //uint
	  fprintf(out_buff, "%u", va_arg(args, int));
	  break;
	case 'x': //hex
	  fprintf(out_buff, "%X", va_arg(args, int));
	  break;
	case 'b': //binary
	  //C doesn't have formatted printing in binary form, so i have to make it myself
	  //i'm printing 8 bits, in the future i might want to look into allowing specification
	  //of number of bits to print
	  unsigned char b = va_arg(args, int);
	  for(int i = 7; i>=0; i--){
	    fprintf(out_buff, "%c", (b & (unsigned int) 1 << i) ? '1' : '0');
	  }
	  break;
	case 'f': //float
	  fprintf(out_buff, "%f", va_arg(args, double));
	  break;
	case 's': //string
	  fprintf(out_buff, "%s", va_arg(args, const char*));
	  break;
	  
	default: //incase the format switch used is not recognized
	  me_print(ME_LOGLEVEL_WARN, "Format switch unrecognized: %c", *str);
	  break;
	}
      }else{
	fprintf(out_buff, "%c", *str);
      }
    }
    va_end(args);
    fprintf(out_buff, "\n");
  }
}

/* prints bert */
void me_print_bert(){
  me_print(ME_LOGLEVEL_ERROR, "BERT");
}
