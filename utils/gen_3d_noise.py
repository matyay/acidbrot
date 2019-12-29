#!/usr/bin/env python3
"""
Generates a 3D noise texture (single channel). VERY memory demanding due to
possible poor implementation of scipy.ndimage.convolve ...
"""
import argparse

import numpy as np
import scipy.signal as sgn
import scipy.ndimage as nd

import matplotlib.pyplot as plt

# =============================================================================

def main():

    # Parse arguments
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter
        )

    parser.add_argument(
        "-s",
        type=int,
        nargs=3,
        required=True,
        help="Texture dimensions (height, width, depth)"
    )

    parser.add_argument(
        "-b",
        type=int,
        required=True,
        help="Gaussian blur radius"
    )

    parser.add_argument(
        "-o",
        type=str,
        default="noise.dat",
        help="Output file"
    )

    args = parser.parse_args()

    # Generate the initial noise
    size  = tuple(args.s)
    noise = np.random.rand(*size).astype(np.float32)

    # Prepare the blurring mask
    blur  = args.b * 2
    sigma = blur / (2 * 4.0)
    w = sgn.windows.gaussian(blur, sigma)

    wy = np.tile(w[:, None, None], (1, blur, blur))
    wx = np.tile(w[None, :, None], (blur, 1, blur))
    wz = np.tile(w[None, None, :], (blur, blur, 1))
    w  = wx * wy * wz;
    w /= np.sum(w.flatten())
    w  = w.astype(np.float32)

    # Blur the noise
    print("Convolving...")
    noise = nd.convolve(noise, w, mode="wrap")

    # Convert to uint
    noise -= np.min(noise.flatten())
    noise /= np.max(noise.flatten())
    noise  = np.clip(noise * 255.0, 0.0, 255.0).astype(np.uint8)

    # Save
    with open(args.o, "wb") as fp:
        data = np.transpose(noise, axes=(2,0,1)).tobytes(order="C")
        fp.write(data)


    print(noise.shape)
    plt.imshow(noise[:,:,0], interpolation="none")
    plt.show()

# =============================================================================

if __name__ == "__main__":
    main()
