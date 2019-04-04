# Backup
The program will only inspect directories specified in the .paths/paths.txt file. To change the root directory of the local and backup, change LOCAL_START and USB_START in macros.h.

Only tested on macOS Mojave 10.14.

## Build/Run instructions
To build, type 'make' in root directory.
To run, type './backup'.

## Options
### Add path to be tracked
```
./backup -a <full_path>
```
### Remove path from tracked directories
Note: Does not remove the directory on the remote directory
```
./backup -r <full_path>
```
