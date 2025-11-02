import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation


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


def animate_samples(samples):
    """
    Animate the samples

    Parameters:
    Array of samples to form animation frames.

    Returns:
    None
    """

    # Create an animation of the samples

    # --- Setup figure and first frame ---
    fig, ax = plt.subplots()
    im = ax.imshow(samples[0], cmap="Greys", vmin=0, vmax=1)
    title = ax.set_title(f"Frame: {0}")
    plt.colorbar(im, ax=ax)

    # --- Update function for animation ---
    def update(frame_idx):
        im.set_array(samples[frame_idx])
        title.set_text(f"Frame {frame_idx}")

    # --- Create animation ---
    anim = FuncAnimation(fig, update, frames=len(samples), interval=50, blit=False)

    plt.show()
