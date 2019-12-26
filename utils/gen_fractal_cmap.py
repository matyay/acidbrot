#!/usr/bin/env python3
import numpy as np
import scipy.ndimage as ndi

from PIL import Image
from matplotlib import cm

# =============================================================================

def gen_stripe(cmap_name, width, height):

    arr = np.linspace(0.0, 1.0, width)
    arr = np.tile(arr[None, :], (height, 1))

    cmap = cm.get_cmap(cmap_name, lut=width)

    image = np.uint8(cmap(arr) * 255.0)
    return image


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

    colormaps = (
        "gnuplot",
        "inferno",
        "cividis",
        "viridis",
        "gist_stern",
        "gnuplot2",
        "rainbow",
        "hot",
    )

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

    # Blur
    h  = np.ones((8, 1))
    h /= 8

    for c in range(image.shape[2]):
        image[:, :, c] = ndi.convolve(image[:, :, c], h)

    image = Image.fromarray(image)
    image.save("colormap.png")

# =============================================================================


if __name__ == "__main__":
    main()
