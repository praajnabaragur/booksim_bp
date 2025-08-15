BookSim Interconnection Network Simulator
=========================================

BookSim is a cycle-accurate interconnection network simulator.
Originally developed for and introduced with the [Principles and Practices of Interconnection Networks](http://cva.stanford.edu/books/ppin/) book, its functionality has since been continuously extended.
The current major release, BookSim 2.0, supports a wide range of topologies such as mesh, torus and flattened butterfly networks, provides diverse routing algorithms and includes numerous options for customizing the network's router microarchitecture.

---

If you use BookSim in your research, we would appreciate the following citation in any publications to which it has contributed:

Nan Jiang, Daniel U. Becker, George Michelogiannakis, James Balfour, Brian Towles, John Kim and William J. Dally. A Detailed and Flexible Cycle-Accurate Network-on-Chip Simulator. In *Proceedings of the 2013 IEEE International Symposium on Performance Analysis of Systems and Software*, 2013.

```bash

# To test unidirectional torus with credits or backpressure
# In src dir (cd src/)
make clean
make

# Modify base_torus_credit.cfg or base_torus_bp.cfg or run the comparison script
chmod +x booksim-credit.sh
chmod +x booksim-bp.sh

./booksim-credit.sh
./booksim-bp.sh

# To plot unidir torus_bp latencies
python3 plot_bp.py

# ---------------
# Also I added a new trafficpattern to select source and dest nodes to route traffic between
# In src dir (cd src/)
chmod +x routing_test_script.sh
./routing_test_script.sh
```