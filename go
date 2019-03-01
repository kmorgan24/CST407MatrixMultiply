#! /bin/bash
R=1000
C=1000
B=1000
mpirun -n 2 ./mpimmult -r $R -c $C -b $B -p 1 > out1
./mmult -r $R -c $C -b $B -p 1 > out2
diff out1 out2