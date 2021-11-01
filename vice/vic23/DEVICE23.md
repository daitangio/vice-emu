# What is Device23?

Device23 is a virtual device which enable pyc64 to do http web requests.
It is mapped as physical device 23 and you use it "opening" web page like:

```basic
  100 print "open test"
  200 open5,23,0,"gioorgi.com"
  300 input# 5,page$
  350 print page$
  400 close 5
```

# About Kernal and Channels
Basic OPEN syntax is
OPEN <logical filenumber> [,<device number> [,<secondary number> [,"<filename>[,<type>[,<mode>]]" ] ] ]

Device number 8 until 15 | disk drive | secondary number 2-14 = data channels; 15 channel for commands


Channel 0 and 1 are officially dedicated to LOAD and SAVE. They differ from normal channels in that the file type is always PRG and the mode is "read" for channel 0 and "write" for channel 1. With this convention the KERNAL does not need to add ",p,r" or ",p,w" to the filename. If this behavior suits your program, nothing stops you to use channel 0 and 1 in your own programs.

Example of floppy disk drive commands
```basic
  open 1,8,15,"n:newdisk,01":close 1        :rem format disk in drive 8 with label newdisk and disk id 01
  open 1,8,15,"s:tempfile":close 1          :rem scratch (delete) the file "tempfile"
  open 1,8,15,"r:newname=tempfile":close 1  :rem renames the file "tempfile" to "newname" - note that the new name comes first
```


From wikipedia about the [KERNAL](https://en.wikipedia.org/wiki/KERNAL):


Device numbers, per established documentation, are restricted to the range [0,16]. However, this limitation came from the specific adaptation of the IEEE-488 protocol and, in effect, applies only to external peripherals. With all relevant KERNAL system calls vectored, programmers can intercept system calls to implement virtual devices with any address in the range of [32,256]. 


# First implementation

First of all we try to use device 8 with secondary channel 3
This test is called test83 and it si very easy to track down.

Channel 23 is difficult to track right now


###### OTHER STUFF TO FIX BELOW

open 1, 8, 8, "gioorgi.com, seq, r" 





To get it working realemulator 



1. intercept calls to $FFCF KERNAL CHRIN 
and try to understand if logical device (A=5 in the example) is mapped. 

It snoops the following page zero locations:

    $B8            Current Logical File Number (5 in the example)
    $B9            Current Secondary Address (0 in the example)
    $BA            Current Device Number
    $BB-$BC        Pointer: Current Filename
    $99            Current  input device
    
When CHRIN routine is called, the next byte of data available from this
device is returned in the Accumulator.
Subsequent calls to this routine will cause the next character in the
line to be read from the screen and returned in the Accumulator, until
the carriage return character is returned to indicate the end of the
line. 


