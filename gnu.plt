set size ratio -1
set xrange [-10:10]
set yrange [-10:10]
set object 1 rect from 0,0 to 1,4 fc lt 2 front
set object 2 rect from -1,1 to 0,5 fc rgb "gold"
set object 5 rect from 0, -3 to 2, -2 fc rgb "cyan" fs pattern 1 bo -1
plot '-' w p ls 1
0 0
e
pause -1
