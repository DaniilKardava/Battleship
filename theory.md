The transition rule is to pick up a ship and place it down randomly anywhere that is available.  
This rule satisfies detailed balance since the distribution $\pi$ is uniform and the transition kernel is symmetric.  

We can similarly show that the Boltzmann distribution satisfies detailed balance in the same sampling scheme.  

Boltzmann weighting can take different forms — one where energy counts pairwise intersections $\binom{n}{2}$,  
a second where it counts the excess $\max(0, n - 1)$, and a third where it counts the indicator of excess  
$\max(0, \min(1, n - 1))$, where $n$ are the occupants on a square.  

Weighting methods 1, 2, and 3 are naturally computed over the squares, but can also be seen as a sum formed  
by placing ships down one by one — adding the new pairwise collisions at each square, or the size of the  
intersection between occupied squares and the new ship, or restricting the previous count to squares that  
had occupancy $1$, respectively.  

We need a further reformulation in how we calculate the marginal weights.  
We noted above that the total board energy can be computed as a sum over consecutively placed ships.  
Now we must compute a single ship’s contribution as a sum over the squares that it occupies.  
The weighting update can then be done one square at a time.  

Note that the active ships list can contain repeats.  
Square values are assumed to always be positive.  

Fractals somehow?  

We can put a crude bound on the average energy.  
The probability that we observe a non–ground state is bounded by  
$\dfrac{w_2}{w_1 + w_2}$ where $w_1$ and $w_2$ are $|\Omega_0|$ and $(T - |\Omega_0|)\, e^{-\beta}$ respectively,  
where $T$ is the total number of states (since $w_2$ is greater than $w_{\text{true}}$ and $x/(1+x)$ is increasing).  
Then the expected energy is bounded above by the max energy times  
$\dfrac{w_1}{w_1 + w_2} \le E_{\max} \cdot \dfrac{T e^{-\beta}}{|\Omega_0|} \le C e^{-\beta}$ for some $\beta$.
