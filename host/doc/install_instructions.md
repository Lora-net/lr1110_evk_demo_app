# LR1110 Host software install instructions

This software is mainly written in python. It requires python to be installed on the machine that will execute the code.

The installation process described here should not require any elevated privileges or super user privileges on Windows.
Python can be installed in the `AppData` folder of normal user.

The final usage of the Lr1110 Host tools still needs a USB access to the embedded Dev Kit.
Therefore you must ensure you company policy and local configuration allow you to plug and use USB devices.

## Pre-requirements

### Ensure you can run python

Open a command line and type:

```bash
python --version
```

You **MUST** have at least Python `3.5` to use this software. If it is the case, go directly to [Ensure you can run pip](#ensure-you-can-run-pip) section.

If it is not the case, there are two possibilities:

1. You get an error on previous command stating you don't have python at all
    1. and you actually never installed python: see [Python is not installed](#python-is-not-installed)
    1. and you did install python but it is not recognize: see [Python is installed but not found](#python-is-installed-but-not-found)
1. You have a python version that is older than `3.5`: see [Python is too old](#python-is-too-old)

### Ensure you can run pip

The `pip` program is a package installer for python. To validate you have pip installed with the correct version, start a shell and enter the following:

```bash
pip --version
```

It must returns a version number, and its location and the python version it is referring to. Ensure that the python version corresponds to the one you plan to use (at least python 3.5), and that it correspond to the python folder you plan to use.

If you get an error at this stage, see section [Issues with Pip](#issues-with-pip).

## Installing the software

> **NOTE** At this point, it considered that you have a correct python installation.

The installation process described here supposes that you have received the *source distribution* of the host software.
This source distribution is typically named `lr1110FieldTest-<version_number>.tar.gz`.
If you do not have it, contact your provider to get it.

In a shell, navigate in the folder that contains the source distribution file and type:

```bash
pip install "lr1110FieldTest-<version_number>.tar.gz"
```

This command must not fail.
If it does fail, ensure the pip program is correctly installed (refers to [previous section](#ensure-you-can-run-pip).
Check that your `PATH` environment variable contains the folder where the pip.exe is installed.

## Checking the installation

### The programs are installed and executable

This project install several projects. To validate the installation, we'll here just ensure that one of them is correctly installed.
To do so, type in a shell:

```bash
Lr1110FieldTest --version
```

It must returns a version number. If it is not the case, see section [Issues with installed software](#issues-with-installed-software).

### The programs can communicate with the Lr1110 dev kit

Here, it is assumed the command `Lr1110FieldTest --version` did return the correct version number.

The purpose of this section is to use the dedicated host software `UsbConnectionCheck` (that has been installed with `Lr1110FieldTest`) and the dedicated embedded firmware `usb_check.bin`.
These dedicated tools implement a light exchange that validates the communication between host and a Nucleo L476RG board.

#### Using the UsbConnectionCheck software

Only a Nucleo L476RG board is required. The Lr1110 shield is not required, nor the touch screen.

First of all, ensure the communication host software is correctly installed by typing the following in a shell:

```bash
UsbConnectionCheck --version
```

The above must return the same version number as for `Lr1110FieldTest --version`.

Then, plug the Nucleo board to your computer using a USB cable, and flash the Nucleo board with the binary `usb_check.bin`.
On Windows, the Nucleo board is identified as a disk in the file explorer where you can simply drag & drop the `usb_check.bin` file.
The Nucleo must starts to flash the MCU with this binary file (the *LD1* LED must blink, indicating the flash is running).

Once the flash is terminated, open a shell and type the following:

```bash
UsbConnectionCheck
```

This command starts the communication check program (it will run forever, you can hit `Ctrl-C` to stop it).

The exchanges that should occur between host and embedded are the following:

1. the embedded code is sending periodically (almost every second) a token on the USB port
1. the host software waits for this message and send back a response (it prints `Got message from embedded. Sending question...` on screen indicating it received the token)
1. the embedded gets the response and send back another message to the host
1. the host gets this new message and prints `Got response from embedded. Communication ok.` to indicate the exchange ran successfully

All the messages are exchanged at 921600 baud.

The trace on the shell must be:

```bash
$ UsbConnectionCheck
Started
Waiting for embedded to send command...
Got message from embedded. Sending question...
Got response from embedded. Communication ok.
Got message from embedded. Sending question...
Got response from embedded. Communication ok.
Got message from embedded. Sending question...
Got response from embedded. Communication ok.
...
```

If you don't see the above trace or get an error, go to section [Issues with UsbConnectionCheck](#issues-with-usbconnectioncheck).

## Troubleshooting

### Issues with Python

#### Python is not installed

This [article](https://realpython.com/installing-python/) provides a good step by step install process for python.
Hereafter is an extract of the main steps.

If you are running Windows:

1. Navigate to [the Python Download page](https://www.python.org/downloads/)
1. Download the installer corresponding to Windows
1. Execute it (it should not require any elevated privileges)

> **IMPORTANT NOTICE** If you are running Windows, don't forget to check the box to *update the PATH environment variable*.
See [this section](#path-environment-variable)

#### Python is installed but not found

If you have python installed but it is not recognize when entering `python --version`, then it is probably because the above important notice has not been followed.

To fix it, you first need to locate your current python install folder. Search for the file `python.exe` which is probably located under a path like:

```bash
C:\Users\<username>\AppData\Local\Programs\Python\Python<version>\
```

After finding it, ensure that it correspond to python version at least 3.5. To do so, either trust the folder path that may display a folder like `PythonX.Y/`, or start a shell, navigate to the folder containing the `python.exe` file, and execute `python --version`.

If you figure out the python version is older than 3.5, then proceed to a complete new python installation following [Python is not installed](#python-is-not-installed).
Your actual python will not be erased.

If the python version you have is of version at least 3.5, then you simply have to update the *PATH* environment variable.
Let's say that the absolute path of you python.exe file is `C:\wonderfull\path\python.exe`. Then you have to add the two following folders to the *PATH* environment variable:

- `C:\wonderfull\path\`
- `C:\wonderfull\path\Scripts\`

#### Python is too old

In this case, you can follow [Install Python](#install-python-case-11). It will not remove the actual install of python.

However, you must ensure that, after the install, the command `python --version` actually calls the newer install (and not the older one).
To do so, simply start a new shell (yes, you must open the shell **AFTER** the end of the installation) and type

```bash
python --version
```

If the python version is the one you just installed, you're good to go.
Otherwise, you will have to modify the *PATH* environment variable so that the new python.exe is found but not the older one.

To do so, simply remove from the *PATH* environment variable the folder corresponding to the old python path, and ensure the folder of the new one is indeed in the *PATH* variable.

After fixing the *PATH*, you must start a new shell and try the command `python --version` again. It must returns the new version of python.

The former contains the python.exe, and the later will contain all command line programs that will be installed by this project.

### Issues with Pip

#### Pip is not installed

If you have python version at least `3.4`, the tool [`ensurepip`](https://docs.python.org/3/library/ensurepip.html) can be used to bootstrap pip.
Running the following command in a shell to use it and install pip (the argument `--default-pip` is here to install the `pip` script, otherwise it install only `pipX` and `pipX.Y`):

```bash
python -m ensurepip --user --default-pip
```

If that still don't work, you have to download the [get-pip.py script](https://bootstrap.pypa.io/get-pip.py) and execute it by

```bash
python get-pip.py
```

Now, typing `pip --version` in a new shell must give you the correct information.

#### Unknown option version

If this command fails with an error similar to:

```bash
$ pip --version
Unknown option: version
```

It is probably that the `pip` command is not the one from Python, but from Perl.
To validate that point, run the following in a shell:

```bash
where pip
```

It should return a path similar to `...\python\python<version>\Scripts\pip.exe`. But in the case of the incorrect `pip`, it may return a path like `...\perl\bin\pip.bat`.

To overcome this issue, you can try an alternate name of Python's `pip` program:

```bash
pip3 --version
```

If the above returns the correct version number associated with the correct python installation, you can keep following this guide by replacing all calls to `pip` program by the alternate name `pip3`.

### Issues with installed software

#### Command not found

If you get an error indicating that the command `Lr1110FieldTest` is unknown, you probably have an issue with the *PATH* environment variable.

To figure it out, try to find the entry point of the `Lr1110FieldTest` software in the python distribution folder.
It probably is under a path like:

```bash
C:\Users\<username>\AppData\Local\Programs\Python\Python<version>\Scripts\Lr1110FieldTest.exe
```

If you do find the *Lr1110FieldTest.exe* at that location, then your *PATH* is probably badly configured.
Modify it to include the folder `C:\Users\<username>\AppData\Local\Programs\Python\Python<version>\Scripts\`.
For more details about the environment variable and how to change it, you can have a look on [https://superuser.com/questions/284342/what-are-path-and-other-environment-variables-and-how-can-i-set-or-use-them](https://superuser.com/questions/284342/what-are-path-and-other-environment-variables-and-how-can-i-set-or-use-them).

If the file is not here, there are several possible problems. Either the install process failed, or you have another python distribution installed, and the software got installed in another place.

In the later, you have to ensure you install the software for the correct python version, with the correct *pip* program.

### Issues with UsbConnectionCheck

#### Issue: No port connected

If you get a python exception trace like:

```bash
$ UsbConnectionCheck
Traceback (most recent call last):
  File ".../UsbConnectionCheck", line 8, in <module>
    sys.exit(entry_point_connection_tester())
  File ".../SerialHandlerConnectionTest.py", line 65, in entry_point_connection_tester
    launch_test()
  File ".../SerialHandlerConnectionTest.py", line 41, in launch_test
    port = serial_handler.discover(SerialHandler.DISCOVER_PORT_REGEXP)
  File ".../SerialHandler.py", line 94, in discover
    regexp)
lr1110evk.SerialExchange.SerialHandler.SerialHandlerExceptionNoPorts: No port connected that could be STM32 (used regexp '(STM.*)|(374B)')
```

It means that there is no Nucleo board detected. It may not be connected.
If it is indeed connected, try with another Nucleo board.

#### Issue: too many ports

If you get a trace like:

```bash
$ UsbConnectionCheck
Traceback (most recent call last):
  File ".../UsbConnectionCheck", line 8, in <module>
    sys.exit(entry_point_connection_tester())
  File ".../SerialHandlerConnectionTest.py", line 65, in entry_point_connection_tester
    launch_test()
  File ".../SerialHandlerConnectionTest.py", line 41, in launch_test
    port = serial_handler.discover(SerialHandler.DISCOVER_PORT_REGEXP)
  File ".../SerialHandler.py", line 97, in discover
    regexp, ports)
lr1110evk.SerialExchange.SerialHandler.SerialHandlerExceptionTooManyPorts: Too many ports could be the STM32 (used regexp '(STM.*)|(374B)'): [...,...]
```

then you may possibly have several Nucleo board connected.
Disconnect all but one, and reflash the one you keep connected with the `usb_check.bin`.

#### Issue: never received message

If the trace stays stuck to the following:

```bash
$ UsbConnectionCheck
Started
Waiting for embedded to send command...
...
```

then it means that either the embedded is broken (firmware is not executing), or the link is broken.
This may also happen if there are several Nucleo Board connected.

Check that the COM port receives messages from the embedded by opening a Terminal Emulator software on the Nucleo COM port configured with 921600 baud.
Check that the word `!TEST_HOST` is received almost every second.

#### Issue embedded -> host

If the trace looks like:

```bash
Unexpected data received from embedded: '<something>'. Communication KO!
```

where `<something>` is any set of characters, then the channel from embedded to host is corrupted, or not on the correct baud.

Check that there is no other tool or shell listening on the Nucleo COM port.
The baud is a hard configuration on the system so it should not be an issue.

#### Issue host -> embedded

If the trace looks like:

```bash
$ UsbConnectionCheck
Started
Waiting for embedded to send command...
Got message from embedded. Sending question...
Got message from embedded. Sending question...
Got message from embedded. Sending question...
...
```

where there is never the line `Got response from embedded. Communication ok.`, it suggests that the embedded never get the response from host (that one that is sent on step 2.).

In this case check the configuration of the COM port.
Ensure the Nucleo board has not been modified to disconnect the Serial 2 from the ST-Link.

## Addendum

### Path environment variable

Any software that is installed on a system can be accessed from a shell by entering its name (and not its complete absolute path) if the folder that contains it is referenced in the *PATH* environment variable.

Therefore, when you hit `python --version` in a shell, the shell search for a file called *python* (or *python.exe*) in all folders referenced by *PATH* environment variable.

In this project there are several programs to consider. First, there is the python interpreter.
Second is the set of command line programs that are installed by this project (Lr1110Demo, Lr1110FieldTest, et al.).
These are called *entry point*s (it is the equivalent of `main` function in C).
All these programs must resides in a folder that is in the *PATH* environment variable in order to be accessible from command line.

Typicals folder paths holding `python.exe` interpreters and *entry point*s are respectively:

-

  ```bash
  C:\Users\<username>\AppData\Local\Programs\Python\Python<version>\
  ```

-

   ```bash
   C:\Users\<username>\AppData\Local\Programs\Python\Python<version>\Scripts
   ```

Therefore, a python-based software that is installed in the `Scripts\` folder is accessible from the shell only if the *PATH* environment variable contains the folder.

If at some point you figure out that an installed python software is not accessible from command line, consider checking the value of the *PATH* environment variable.

## Additional resources

- [https://docs.python.org/3.7/using/windows.html](https://docs.python.org/3.7/using/windows.html)
- [https://packaging.python.org/tutorials/installing-packages/](https://packaging.python.org/tutorials/installing-packages/)
