# MEDIT

*Portmentau of MEtadata + EDITor*

A terminal-based file metadata editor. The aim is to be very detailed, letting the user perform various actions, like creating and deleting chunks with the option to customize as many options in them as the format can, and in as many different formats as i can.
Thus far, i've been focusing on the LIST INFO format because it's very simple.

### THIS IS NOT A FUNCTIONAL VERSION YET

All this version can do thus far is print metadata chunk info and their fields found in a file, and remove fields. And to see said info you must use the `--loglevel debug` flag.
AAAAAND it only works with INFO and partially with ID3v2

I had started this project in 2025, making it in C++ with TagLib but some TagLib quirks started to annoy me and OOP also started to annoy me, so i decided i was going to make everything myself.

## COMPILING

Just run `make`.
This application does not have any library dependencies.
If you wish you can run `make debug` to get debugging features like asan and make it usable on gdb.

## USAGE

This application is meant to be run as a command from the terminal, example:
`medit -i [files]`

The flags are:

`-i`   specify files to be read.
`--loglevel`   specifies log level the options are:
	       `error`
	       `warn`
	       `info`
	       `debug`
	       `none`
	       You might want to set it to debug if you wanna see any actual output

`--remove_field` removes a field (only works with LIST INFO) you can either specify one field to remove accross many files or specify to field to remove per file
