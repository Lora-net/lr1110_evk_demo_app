# LR1110 evaluation kit

## Introduction

This repository provides everything you need to use a LR1110 evaluation kit.

## Applicative Host

This project needs an applicative companion software to be running on a computer.
This section describes the steps to build/install/use it.

### Build & Install

To build the host softwares, proceed as follow:

```bash
$ cd $TOP_DIR/host
$ python ./setup.py sdist # Build the source distribution archive
$ python -m pip install dist/<BUILT_SOURCE_DIST>.tar.gz # Install the previously built source distribution
```

### Usage

For the demo mode, the applicative companion software is `Lr1110Demo` that is called as follow:

```bash
$ Lr1110Demo <INITIAL_SOLVER_SEARCH_COORDINATE> <EXACT_COORDINATES> -p <HTTP_PORT> -b 921600 -r <ID_SAMPLE> <AUTHENTICATION_TOKEN>
```

In case of need, a short help page is available:

```bash
$ Lr1110Demo --help
```

## Wiki

The Wiki will give you all the information about this evaluation kit:

 - [Software packages](https://github.com/Lora-net/lr1110_evk/wiki/Software-packages)
 - [Demonstration examples](https://github.com/Lora-net/lr1110_evk/wiki/Demonstration-examples)
 - [Updater tool](https://github.com/Lora-net/lr1110_evk/wiki/Updater-tool)
