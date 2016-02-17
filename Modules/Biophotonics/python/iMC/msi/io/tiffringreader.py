'''
Created on Nov 2, 2015

@author: wirkert
'''

import os
import logging

import Image
import scipy
import numpy as np

from msi.io.reader import Reader
from msi.msi import Msi


class TiffRingReader(Reader):
    '''
    TODO SW: document and test
    '''

    def __init__(self):
        pass

    def read(self, fileToRead, n, resize_factor=0.5, segmentation=None):
        """ read the msi from pngs.
        The fileToRead is the first file to read,
        then n files will be read to one msi from a
        sorted file list

        segmentation: tiff filename of the segmentation. If none, it will be
            tried to get a segmentation from npy files with filenames like the
            tiff files + _seg.tiff. If this fails, no segmentation will be
            assumed
        """

        path, file_name = os.path.split(fileToRead)
        files = os.listdir(path)
        files_in_folder = [os.path.join(path, f) for f in files if
                           os.path.isfile(os.path.join(path, f)) and
                           f.endswith('.tiff')]

        files_in_folder.sort()
        position = files_in_folder.index(fileToRead)
        # take n images from found position
        image_array = [to_image(f, resize_factor)
                       for f in files_in_folder[position:position + n]]
        image = reduce(lambda x, y: np.dstack((x, y)), image_array)

        # in case of 1 dimensional image: add a fake last dimension, since
        # we always assume the last dimension to be the wavelength domain.
        # TODO SW: Test this and implement for other readers
        if n is 1:
            image = np.expand_dims(image, -1)

        msi = Msi(image)

        # we pass an explicic image as segmentation
        if segmentation is not None:
            segmentation = to_image(segmentation, resize_factor)
        else: # otherwise: search for numpy segmentations
            try:
                segmentation_array = [to_segmentation(f)
                                      for f in
                                      files_in_folder[position:position + n]]
                if do_resize(resize_factor):
                    segmentation = reduce(lambda x, y: x & y, segmentation_array)
                    segmentation = scipy.misc.imresize(segmentation, resize_factor,
                                                       interp="bilinear")
            except:
                logging.info("didn't find segmentation for all images")
        return msi, segmentation


def do_resize(resize_factor):
    return not np.isclose(resize_factor, 1.0) and (resize_factor is not None)


def to_image(f, resize_factor):
    im = Image.open(f)
    im_array = np.array(im)
    im_array >>= 4

    if do_resize(resize_factor):
        im_array= scipy.misc.imresize(im_array, resize_factor,
                                     interp="bilinear", mode="F")
    return im_array


def to_segmentation(f):
    seg = np.load(f + ".seg.npy")
    return seg
