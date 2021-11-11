

;c162021 ==1001==
   10 rem tune delay?
   20 dx%=1.6
   30 rem anim frames?
   40 rem first number is a float with
   50 rem <duration>,frames...,end
   60 data 35, wellcome,to,the,simple,animation,demo
   70 data with parametric,data animation,
   80 data end
   90 data 3 ,"JQK",end
  100 data 2 ,"UQI", end
  110 data 1 ,"JQK",end
  120 data 1 ,"UQI", end
  130 data 1 ,"JQK",end
  140 data 1 ,"UQI", end
  150 data 1 ,"JQK",end
  160 data 1 ,"UQI",end
  170 data 1 ,"JQK",end
  180 data 1 ,"UQI",end
  190 data 1 ,"JQK",end
  200 data 1 ,"UQI",end
  210 data 1 ,"JQK",end
  220 data 6 ,"UQK happy birthday gio","JWK","3-frame data",end
  230 data 0,aniend
  240 remscnclr
  250 for c2=1 to  2
  260 ti$="000000"
  270 do
  280 read dur%
  290 if dur% = 0 then exit
  300 scnclr
  310 print
  320 do
  330 read anim$
  340 if anim$="end" then exit
  350 print anim$
  360 loop
  370 print "{home}",ti$,"    d:",dur%
  380 rem how much wait?
  390 tstart=ti
  400 do while ti < (tstart+ dur%*dx% )
  410 rem debug:
  420 print"{home}",ti$,(tstart+dur%*dx%)-ti
  430 loop
  440 loop: rem new frame loop
  450 rem  scnclr
  460 restore
  470 next c2
  480 scnclr
  490 print "backup..."
  500 dsave "c162021"

