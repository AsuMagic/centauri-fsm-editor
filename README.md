# Planet Centauri FSM editor

This is a WIP tool to visually edit FSMs (Finite State Machines) for use in Planet Centauri. This is useful to edit monster logic in a visual and user-friendly way.

## Building

At the moment, you should use `vcpkg` to install dependencies for the standalone application.

1. Follow the [quick start guide](https://github.com/microsoft/vcpkg#getting-started) for `vcpkg` depending on your platform.
2. Install `sfml`, `imgui-sfml` and `imgui`: `vcpkg install sfml imgui-sfml imgui`. You can use [this](https://vcpkg.readthedocs.io/en/latest/examples/installing-and-using-packages/) as a reference to install packages using `vcpkg`.
3. `mkdir build && cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -GNinja`.
4. You can now build by running `ninja` inside of the `build/` directory.

## Internal docs

A Doxygen documentation is provided to help understand the internals of the FSM editor and is a good starting point.  
At the moment, you need to build the documentation seperately:

```sh
cd doc/out
doxygen ../Doxyfile
```

You can then open the HTML docs from `doc/out/html/index.html`.