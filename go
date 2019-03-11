#! /bin/bash
R=7
C=7
B=7
mpirun --hostfile ./hostfile ./mpimmult -r $R -c $C -b $B -p 1 > out1
./mmult -r $R -c $C -b $B -p 1 > out2
diff out1 out2
