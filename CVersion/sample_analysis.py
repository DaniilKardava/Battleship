import numpy as np

values = []
with open("samples.txt") as f:
    for chunk in f.read().split(","):
        if chunk:  # avoid empty strings at the end
            values.append(int(chunk))
data = np.array(values, dtype=np.int32)

n = 20
iters = 3000 

data = data.reshape(iters, n, n)

data = np.sum(data, axis = 0) / n 

from matplotlib import pyplot as plt 

def make_heatmap(grid_averages):
    """
    Make heatmap of grid averages

    Paramaters:
    Accepts grid-like array.

    Returns:
    None
    """

    # Plot table and heat map

    plt.imshow(grid_averages, cmap="viridis")
    plt.colorbar(label="Value")

    plt.show()

make_heatmap(data)