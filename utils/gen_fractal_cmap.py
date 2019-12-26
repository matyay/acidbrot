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
    return image[:, :, :3]


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
        "spring",
        "inferno",
        "cool",
        "cividis",
        "viridis",
        "gist_rainbow",
        "gnuplot2",
        "rainbow",
        "hot",
        "Wistia",
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

    # Saturate
    for y in range(image.shape[0]):
        for x in range(image.shape[1]):
            pel = image[y, x, :].astype(np.float32) / 255.0

            d   = pel - np.array([0.5, 0.5, 0.5])
            d  *= 1.5;
            pel = np.clip(np.array([0.5, 0.5, 0.5]) + d, 0.0, 1.0)

            image[y, x, :] = (pel * 255.0).astype(np.uint8)

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
