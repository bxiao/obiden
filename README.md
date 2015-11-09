# obiden
A modified RAFT consensus algorithm with dual leaders

The algorithm attempts to address the single leader bottleneck with a single simple change. Allow a single host to help the leader communicate with the other hosts. Thus the leader is now called the president, and the helper is called the vice-president. There are several implications to this change.

1. The heartbeat and data have to be propagated through the vice president fast enough to prevent an election from starting.
2. If the vice-president fails, there needs to be a mechanism to prevent the hosts who stopped receiving the propagated heartbeat from attempting to elect another president. If the hosts know who the president is, they can send a message to the president which can send a heartbeat (or data) response to prevent the election until a new vice-president is elected.
3. If the vice-president is in some way closer (for instance has lower latency or a more robust connection) to the president it should help with performance or availability.
4. Due to the previous point and the fact that the president communicates through it, it is reasonable to assume that the vice-president is likely to be close to “caught up” with the president. This makes it a good choice for president, should the current one fail. Rather than try to guarantee this, we give the vice-president an advantage during the election (minumum election timeout) and then let the normal Raft process determine the new president.
5. If the vice-president fails or a message to the vice-president is lost, it must be ensured that the hosts who receive messages from the vice-president do not attempt to elect a new president. Also the lost message(s) must be resent directly from the leader until a new vice-president is appointed.

The Obiden algorithm attempts to make as few changes to Raft as possible while still dealing with the above ramifications to the addition of the vice-president role. It represents a weakening of the strong leader, but not in a way that has been done in other research. In other weak leader algorithms, decision-making authority over the state is delegated to the hosts, in Obiden it stays with the president. The vice-president simply helps to get the word out to the other hosts. We believe this makes the Obiden algorithm unique.

We believe our algorithm is nearly as simple as the Raft algorithm, in fact it can be compatible with the Raft algorithm; in the case of a small number of servers an Obiden implementation should reduce to a Raft implementation since the leader bottleneck won’t exist. More importantly, to ensure correctness, in the case that the president does not have a currently appointed vice-president, it sends messages to all hosts, thus reducing to the Raft algorithm. However, for larger clusters, the Obiden algorithm should allow faster throughput by sharing the load of communicating with tol of the hosts with the vice-president.

(Still a work in progress...)
