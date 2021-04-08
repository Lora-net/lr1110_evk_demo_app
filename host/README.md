# LR1110 Host software readme

## Install instruction

Refer to [dedicated document](doc/install_instructions.md).

Basically:

```bash
python setup.py
pip install dist/lr1110evk-<version>.tar.gz
```

## Lr1110Demo

This program is the companion software of the demonstration using LR1110 EVK hardware.
The LR1110 EVK hardware must be connected to the computer running the Lr1110Demo through USB cable.

The goal of Lr1110Demo is to provide the date en assisted location information to the Lr1110.
It also propagate the calls to cloud solvers to solve the location from the MAC addresses and NAV message scanned by the LR1110.

### Usage

The up-to-date usage can be obtained by running the following:

```bash
$ Lr1110Demo -h
<USAGE INSTRUCTIONS>
```

A basic usage of the Lr1110Demo is:

```bash
Lr1110Demo -d <COM PORT> <GNSS SOLVER ASSISTANCE LOCATION> <EXACT LOCATION> <GLS TOKEN>
```

With:

- `<COM PORT>`: The com port where the LR1110 EVK is connected (something like */dev/ttyACM0* on linux, *COM10* on Windows);
- `<GNSS SOLVER ASSISTANCE LOCATION>`: The assistance coordinate provide to the GNSS solver when requesting a location solving. Format is `latitude,longitude,altitude` expressed in decimal degree. Note there are no space characters around the `,` to avoid breaking the command line interpretation;
- `<EXACT LOCATION>`: The exact location where the scan operation occured. The format is the same as `<GNSS SOLVER ASSISTANCE LOCATION>`;
- `<GLS TOKEN>`: The authentication token on GLS server.

**NOTE**: If one of the location has a negative latitude, then its value on the command line will starts with a `-` character. It will therefore be interpreted as an optional argument (as for `-d <COM PORT>`). Moreover, as it will certainly refers to a non existing optional argument, command line parsing will fail with an error similar to

```bash
$ Lr1110Demo -b COM10 -1.0000,0.0000,0 -1.0000,0.0000,0 <GLS TOKEN>
usage: Lr1110Demo [-h] [-s WIFI_SERVER_BASE_URL] [-p WIFI_SERVER_PORT]
                  [-t GNSS_SERVER_BASE_URL] [-q GNSS_SERVER_PORT]
                  [-d DEVICE_ADDRESS] [-b DEVICE_BAUD] [-n]
                  [-l GNSS_ASSISTED_SCAN_APPROXIMATE_LOCALIZATION] [-r]
                  [-F FAKE_DATE] [--verbose] [--version]
                  approximateGnssServerLocalization exactCoordinate
                  glsAuthenticationToken
Lr1110Demo: error: the following arguments are required: glsAuthenticationToken
```

To avoid the bad interpretation of the coordinate as optional argument, you must use the `--` separator between optional and positional arguments as follows:

```bash
Lr1110Demo -b COM10 -- -1.0000,0.0000,0 -1.0000,0.0000,0 <GLS>
```

See guideline 10 of [that reference](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html).