# Mutate

`mutate` is a command line tool for generating many new versions of images by applying and compositing random image operations. This can be useful when generating large amounts neural networking training data, which is exactly why the tool was created.

## Example usage

Create 10 new version of input.jpg, will create input-1.jpg, input-2.jpg, ..., input-n.jpg.

$ mutate -n 10 input.jpg

Create 1 new version of input.jpg, applying 50 random operations before saving:

$ mutate -n 1 -p 50 input.jpg

Create 10,000 new versions of input.jpg, slightly lowered chaos level and 15 operations per image, save the results in the directory /srv/storage0:

$ mutate -n 10000 -p 15 -c 0.15 input.jpg -o /srv/storage0

## Building and dependencies

`mutate` depends on ImageMagick/MagickWand.

Debian users:

```
$ sudo apt-get install libmagickwand-dev
```

Building and running:

```
$ make
$ ./mutate -n 1 -p 10 input.jpg
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

Copyrigt (c) 2017 Teorem AB

Mutate is free and licensed under the MIT license.
