import numpy as np

values = []
with open("energies.txt") as f:
    for chunk in f.read().split(","):
        if chunk:  # avoid empty strings at the end
            values.append(int(chunk))
energies = np.array(values, dtype=np.int8)

iterations = 1000
energies = energies.reshape(-1, iterations)
avg_energies = np.mean(energies, axis = 1)
energy_variance = np.var(energies, axis = 1) / iterations


from matplotlib import pyplot as plt

print(avg_energies)
plt.show()

x = np.arange(0,10, 0.01)
plt.plot(x, avg_energies)
plt.plot(x, avg_energies + energy_variance)
plt.plot(x, avg_energies - energy_variance)
plt.show()

# Integrate 
delta = np.sum(0.01 * avg_energies)
print(delta)
