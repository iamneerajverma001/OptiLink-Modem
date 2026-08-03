# Configuration Reference

## TX parameters

| Parameter | Purpose |
|---|---|
| frame length | controls payload size per frame |
| sync pattern | helps the receiver detect the start of a frame |
| symbol period | sets timing for each symbol |
| amplitude | controls transmit strength |
| packet spacing | controls time between packets |
| encoding mode | selects the modulation or symbol scheme |

## RX parameters

| Parameter | Purpose |
|---|---|
| threshold | determines symbol decision |
| sampling offset | shifts the sampling instant |
| timing recovery gain | controls how quickly timing is corrected |
| sync detection window | controls how much signal is considered for sync |
| packet timeout | determines how long to wait before resetting |
| lock threshold | defines when lock is considered stable |

## Key equations

$$t_k = kT_s + \tau$$

$$\tau_{n+1} = \tau_n + \mu e[n]$$

$$\hat{x}_k = \begin{cases}
1, & r_k > V_{th} \\
0, & r_k \le V_{th}
\end{cases}$$
