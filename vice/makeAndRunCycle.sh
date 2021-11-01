#!/bin/bash
#make -j 2 && sudo make install && ( cd vic23/; ../src/xplus4 -config vic23.settings )

make -j 3 && ( cd vic23/; ../src/xplus4 -config vic23.settings )