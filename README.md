# HBSX
A (work in progress) homebrew app store for the original Xbox.

## How to build
To build, you will need [nxdk](https://github.com/XboxDev/nxdk).

Clone the repository, with submodules, and run `./build.cmd`. You may also need to change the location of nxdk in the Makefile. The expected project path is `nxdk-install/projects/HBSX`.

```sh
git clone https://github.com/ShapedFox/HBSX.git --recursive
cd HBSX
./build.cmd
```

The compiled executable (default.xbe) is located in the newly-created `bin` folder. You will also find a .ISO file in the project root.

There is also an option to, after compilation, start [xemu](https://github.com/xemu-project/xemu). (You will have to manually open the .ISO using the toolbar though. And for it to even run you must declare XEMU_EXEC as an environment variable.)

```sh
./build.cmd XEMU
```
