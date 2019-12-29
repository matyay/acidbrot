#!/usr/bin/env python3
import random
import numpy as np
import scipy.ndimage as ndi

from PIL import Image
from matplotlib import cm

# =============================================================================

def gen_stripe(cmap_names, width, height):

    # Single colormap
    if isinstance(cmap_names, str):
        N = 1
        cmap_names = (cmap_names,)

    # Multiple colormaps to concatenate
    else:
        N = len(cmap_names)

    image = None
    for i in range(N):
        part = np.linspace(0.0, 1.0, width // N)
        part = np.tile(part[None, :], (height, 1))

        name = cmap_names[i].replace("~", "")
        flip = cmap_names[i].startswith("~")
            
        cmap = cm.get_cmap(name, lut=width)
        part = cmap(part)

        if flip:
            part = part[:, ::-1]

        if image is None:
            image = part
        else:
            image = np.hstack((image, part))

    # Convert to uint8
    image = np.uint8(image * 255.0)
    return image[:, :, :3]

def saturate(image, fac):

    image  = image.astype(np.float32) / 255.0
    image -= 0.5
    image *= fac
    image += 0.5
    image  = np.clip(image, 0.0, 1.0) * 255.0

    return image.astype(np.uint8)

def main():

#    colormaps = (
#        "twilight",
#        "twilight_shifted",
#        "hsv",
#        "terrain",
#        "gist_stern",
#        "gnuplot",
#        "gist_rainbow",
#        "rainbow",
#        "jet",
#        "nipy_spectral",
#        "gist_ncar",
#    )

    # List of available colormaps
    available_colormaps = (
        "Purples",
        "Blues",
        "Greens",
        "Oranges",
        "Reds",
        "PuRd",
        "BuPu",
        "GnBu",
        "PuBu",
        "YlGnBu",
        "YlGn",
        "viridis",
        "plasma",
        "inferno",
        "magma",
        "cividis",
        "gray",
        "bone",
        "pink",
        "hot",
        "gist_heat",
        "copper",
        "twilight_shifted",
        "gist_earth",
        "gnuplot",
        "gnuplot2",
        "CMRmap",
        "cubehelix",
        "rainbow"
    )

    # Add reverses and shuffle
    colormaps = list(available_colormaps) + \
                ["~" + m for m in available_colormaps]

    random.shuffle(colormaps)

    # Generate
    image = None
    for colormap in colormaps:
        
        # Generate a stripe
        stripe = gen_stripe(colormap, 256, 8)

        # Concatenate
        if image is None:
            image = stripe
        else:
            image = np.vstack((image, stripe))

    # Saturate
    image = saturate(image, 1.5)

    # Blur
    h  = np.ones((7, 1))
    h /= np.sum(h.flatten())

    for c in range(image.shape[2]):
        image[:, :, c] = ndi.convolve(image[:, :, c], h, mode="wrap")

    image = Image.fromarray(image)
    image.save("colormap.png")

#    import matplotlib.pyplot as plt
#    plt.imshow(image, interpolation="none")
#    plt.show()

# =============================================================================


if __name__ == "__main__":
    main()
