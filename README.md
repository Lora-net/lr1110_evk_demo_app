
# LR1110 evaluation kit - demonstration application

## Introduction

This repository proposes a demonstration application running on a **LR1110 evaluation kit**.

Additional assets can be found in [the Wiki](https://github.com/Lora-net/lr1110_evk_demo_app/wiki/home) (software packages, command tool, etc.).

## Applicative Host

When using a **LR1110 transceiver**, an applicative companion software running on a computer shall be used.

This section describes the steps to build/install/use it.

### Build & Install

To build the host software, proceed as follow:

```bash
$ cd $TOP_DIR/host
$ python ./setup.py sdist # Build the source distribution archive
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

### AlmanacUpdate Usage

The `AlmanacUpdate` command line program is to be used to execute a full almanac update operation of the LR1110 (both transceiver and modem).

It can use almanac coming either from a DAS server or from a GLS server (default is DAS server).

To use it, execute the following in a command line:

```bash
$ AlmanacUpdate -h
# Display the help message with all possible flags

$ AlmanacUpdate -d <EVK_COM_PORT> <DAS_TOKEN>
# Update the almanac of an evaluation kit connected on <EVK_COM_PORT> using DAS server

$ AlmancUpdate -g -d <EVK_COM_PORT> <GLS_TOKEN>
# Update the almanac of an evaluation kit connected on <EVK_COM_PORT> using GLS server
```

A correct execution of the almanac update operation is determined by the trace being terminated with message *Check terminated*.

*Note: this command can also be used to update the almanacs of a **LoRa Basics Modem-E**.*
