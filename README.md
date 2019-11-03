# DFU example

@note This example supports nRF52832 Series DK pca10040 and Thingy-52 pca20020.

This Device Firmware Update (DFU) example illustrates how to create an application that can be updated
over the mesh using background mode DFU. In this mode, the new firmware is transferred in the background
while the application is running and the DFU reports to the application when the transfer is done.
The application can then flash the new firmware when ready.

---

## Software requirements 

This DFU application requires:
  * nrf52 SDK version 15.3.0 (SDK_ROOT env var)
  * nrf52 Mesh SDK version 3.2.0 (SDK_MESH env var)
  * nrf52 Thingy-52 SDK version 2.1 (SDK_THINGY52 env var)
  * nrfjprog binaries
  * nrfutilmesh binary
  * Sagger IDE with its built-in ARM compiler
  * make, bash

---

## Initial Setup

1) Edit the script file `setEnv` and adapt the full path location for SDK_ROOT, SDK_MESH and SDK_THINGY52

The run it from shell like '. setEnv'. This will modify all .emProject files with correct sdk locations. You can run this script every time on a new shell to set env variables.

2) Run the script file `fixSDK`.

This is needed only once. It will apply code fixes to sdk (there are some issues with Logger API and with conflicting definition of pca20020 board). 


## Fast Demo 

Precompiled scenarios for DFU are included in the `demo` directory at the project root directory.

Later compiling of Release or Debug profiles re detected and used by the flashing scripts.

### Scenario 0: - Initial DFU FW setup on DK and on Thingies

1) run `program-dk` script giving the necessary arguments to flash inital FW on the DK alone.

```
e.g. ./program-dk -s 682257226 -v 0  
```
Where `-s` means the DK serial number and `-v 0` means we run Scenario 0. This script assumes the tty to be ttyACM0 but you can customize it with the `-t` optional parameter.

2) run `program-52` script giving the necessary arguments to flash inital FW on the Thingy-52. Run this for each Thingy device you have.
Precondition is to connect the Thingy Debug-In to the DK Debug-Out before running the script.

```
e.g. ./program-52 -s 682257226 -v 0  
```
Where `-s` means the DK serial number and `-v 0` means we run Scenario 0. This script assumes the tty to be ttyACM0 but you can customize it with the `-t` optional parameter.

3) You can observe via a BLE scanner mobile app that the nodes show up. ( e.g. with Nordic nRF Connect Android/iOS App ) But this FW has no features for provisioning, so your mobile app `nRF Mesh` will not detect them.


### Scenario 1: - Custom serial DFU FW upgrades on DK and on Thingies
Precondition is to make sure there is nothing connected to the Debug-Out of the DK board.

1) run `program-dk` script giving the necessary arguments to DFU Upgrade on the DK alone.

```
e.g. ./program-dk -s 682257226 -v 1  
```
This will take a while(~40mins), you will see a progres indicator. During DFU the DK board keeps Led 0 and Led 2 on. The DFU packets are also relaied but thingies will not upgrade because the package is not intended for them. 

At the end the DK will automatically reset and start blinking Led 0 as a sign that new app is running ( and that is not provisioned in a mesh)

2) run `program-52` script giving the necessary arguments to DFU Upgrade on the Thingies. This uses the DK again for serial-DFU. Also nothing has to be connected the the DK Debug-Out.

```
e.g. ./program-52 -s 682257226 -v 1  
```

During DFU the dk will keep Led 2 and Led 3 on ( another sign that the DK has a different App running on it).
Also all the Thingies will breath YELLOW as a sign that this upgrade is intended for them.

At the end the Thingies will automatically reset and start breathing RED as sign that they are not yet provisioned in a Mesh.



### Provisiong the Demo

Open now the mobile app 'nRF Mesh' and add and provision the new devices ( they are now discoverable via GATT ). Notice when you provision a Thingy it will turn Blue.
If you reset the provisioning it will turn again Red.


## Compiling

### Compiling from IDE

You can compile from Segger IDE given the setEnv was run before.

### Compile from command line

TODO...
