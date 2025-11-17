import numpy as np

values = []
with open("energies.txt") as f:
    for chunk in f.read().split(","):
        if chunk:  # avoid empty strings at the end
            values.append(int(chunk))
energies = np.array(values, dtype=np.int8)

iterations = 100
energies = energies.reshape(-1, iterations)
avg_energies = np.mean(energies, axis = 1)
energy_variance = np.var(energies, axis = 1) / iterations


from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

x = np.arange(0,10, 0.1)

# your model
def model(x, alpha, beta):
    return alpha * np.exp(beta * x)

p0 = [1.0, -1.0]

# perform the fit
popt, pcov = curve_fit(model, x, avg_energies, p0=p0)

alpha, beta = popt
print(f"a = {alpha}, b = {beta}")

plt.plot(x, alpha* (np.exp(x) ** beta))
plt.plot(x, avg_energies)
plt.show()

# Integrate 
delta = np.sum(0.01 * avg_energies)
print(delta)

import sympy as sp

x = sp.symbols('x')
f = alpha * sp.exp(x * beta)

# definite integral
I = sp.integrate(f, (x, 0, sp.oo))
print(I)   