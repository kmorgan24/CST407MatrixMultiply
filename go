#! /bin/bash
R=4
C=4
B=4
mpirun -n 2 ./mpimmult -r $R -c $C -b $B -p 1 > out1
./mmult -r $R -c $C -b $B -p 1 > out2
diff out1 out2