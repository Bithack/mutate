# Mutate

`mutate` is a command line tool for generating many new versions of images by applying and compositing random image operations. This can be useful when generating large amounts neural networking training data, which is exactly why the tool was created.

![Example output images](/example.jpg?raw=true "Example output images")

## Example usage

Create 10 new version of lena.jpg, will create lena-1.jpg, lena-2.jpg, ..., lena-n.jpg.

```
$ mutate -n 10 lena.jpg
```

Create 1 new version of lena.jpg, applying 50 random operations before saving:

```
$ mutate -n 1 -p 50 lena.jpg
```

Create 10,000 new versions of lena.jpg, slightly lowered chaos level and 15 operations per image, save the results in the directory /srv/storage0:

```
$ mutate -n 10000 -p 15 -c 0.15 lena.jpg -o /srv/storage0
```

The montage in this README was generated with:

```
$ mutate -n 42 -c 0.15 lena.jpg -o out/
$ montage out/*.jpg example.jpg # imagemagick tool
```

## Building and dependencies

`mutate` depends on ImageMagick/MagickWand.

Debian users:

```
$ sudo apt-get install libmagickwand-dev
```

Building and running:

```
$ make
$ ./mutate -n 1 -p 10 lena.jpg
```

## Operations applied by `mutate`

List of planned or implemented operations applied by mutate:

Color operations:

* [X] Contrast normalization
* [X] Contrast stretching
* [X] Histogram equalization
* [ ] Midtone color tinting
* [ ] Modulation
* [X] Hue modulation
* [ ] Constant add/subtract
* [X] Solarize
* [ ] Color LUT
* [ ] Grayscale
* [ ] Levels
* [X] Auto levels
* [X] Auto gamma
* [X] Sigmoidal contrast

Geometric operations

* [X] Rotation
* [ ] Zoom
* [X] Perspective
* [X] Wave
* [X] Implode
* [X] Swirl

Misc operations

* [ ] Tilt-Shift
* [ ] Pixelate
* [ ] Chroma Key Masking
* [ ] Edge detection
* [ ] Add background
* [X] Blur
* [ ] Hardlight fabric
* [X] Pencil sketch

## Authors

* Emil Romanus <emil.romanus@teorem.se>


## License

Copyright (c) 2017 Teorem AB

Mutate is free and licensed under the MIT license.
