

;c162021 ==1001==
   10 rem anim frames?
   20 data 4
   30 rem first number is a float with
   40 rem <size>,<duration>,...anim
   50 data 1,2 ,"UQI"
   60 data 1,2 ,"JQK"
   70 data 3,5  ,"UQK happy birthday gio","JWK","3-frame data"
   80 data 1,0,"end"
   90 scnclr
  100 for c2=1 to  2
  110 read frames%
  120 forc= 1 to frames%
  130 rem *single frame renderning*
  140 read anisize,dur%
  150 for s= 1 to anisize
  160 read anim$
  170 print anim$
  180 next s
  190 print:print fre(0),ti$,"d:",dur%
  200 rem how much wait?
  210 tstart=ti
  220 do while ti < (tstart+ dur%*5   )
  230 rem wait
  240 loop
  250 scnclr
  260 next c
  270 restore
  280 next c2
  290 scnclr
  300 print "backup..."
  310 dsave "c162021"

