# LR1110 evaluation kit - demonstration application

## Introduction

This project is a demonstrator application for LR1110 Transceiver and LoRa Basics Modem-E.

It provides embedded example code in `embedded/` folder and companion software tools in `host/` folder.
The companion tools are to be used specifically with LR1110 Transceiver, except `AlmanacUpdate` that can be used with both LR1110 Transceiver and LoRa Basics Modem-E.

Additional assets can be found in [the Wiki](https://github.com/Lora-net/lr1110_evk_demo_app/wiki/home) (software packages, command tool, etc.).

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
$ Lr1110Demo <INITIAL_SOLVER_SEARCH_COORDINATES> <EXACT_COORDINATES> -p <HTTP_PORT> -b 921600 -r <ID_SAMPLE> <AUTHENTICATION_TOKEN>
```

In case of need, a short help page is available:

```bash
$ Lr1110Demo --help
```

### AlmanacUpdate Usage

The `AlmanacUpdate` command line program is to be used to execute a full almanac update operation for both LR1110 Transceiver and LoRa Basics Modem-E.

It can use almanac coming either from a DAS server or from a GLS server (default is DAS server).

To use it, execute the following in a command line:

```bash
$ AlmanacUpdate -h
# Display the help message with all possible flags

$ AlmanacUpdate -d <EVK_COM_PORT> <DAS_TOKEN>
# Update the almanac of an EVK connected on <EVK_COM_PORT> using DAS server

$ AlmanacUpdate -g -d <EVK_COM_PORT> <GLS_TOKEN>
# Update the almanac using GLS server
```

A correct execution of the almanac update operation is determined by the trace being terminated with message *Check terminated*.
