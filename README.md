# swap-interval-test

## It is absolutely recommended that you read the entire document over at least once to get a better idea of what is going on!

## Keep an eye on the terminal when you run it at the very least!

### Preamble

There seems to be a bug in some deployments in the wild of GLFW on Linux
where using the call `glfwSwapInterval` will cause the application in
question to become unreasonably "laggy" when the window is dragged. This
repository exists as a tool to see if you are in fact impacted by the issue.

### Details

This tool comes in three modes `GLFW_FIXED` environment variable modes,
`FALSE` `HALF` `TRUE`. This is how systems should behave based on whether
they're affected and which mode they are using.

| `GLFW_FIXED` | Affected Systems                    | Unaffected Systems                  |
|--------------|-------------------------------------|-------------------------------------|
| `FALSE`      | Laggy dragging, No thread overload  | Normal dragging, No thread overload |
| `HALF`       | Normal dragging, Thread overload    | Normal dragging, Thread overload    |
| `TRUE`       | Normal dragging, No thread overload | Normal dragging, No thread overload |

### Disclaimer

This fix implemented in the program should NOT be used on a unaffected system,
only avoid `glfwSwapInterval(1)` on an affected system. Unaffected systems SHOULD
use `glfwSwapInterval(1)`!

### Building & Running

Works on my machine™

See above for what you should put for the values of `GLFW_FIXED` and what results
to expect!

```bash
mkdir build
cd build
cmake ..
make
GLFW_FIXED=FALSE ./swap-interval-test
```

### Explanation

For whatever reason, for specific hardware configurations `glfwSwapInterval(1)` is just
plain busted and breaks smooth window dragging. No idea why. The first and biggest lead
here is probably [the issue on glfw's issue tracker](https://github.com/glfw/glfw/issues/1016).

The fix here really isn't anything special- it just has the main thread wait to limit the
frames per second the application is rendered at. The reason why not just disabling the
swap interval is a complete solution (despite "working") is because it will exhaust the
thread the program is running on, giving the program no time to sleep.

### Sightings

If you have a system configuration **where window dragging is broken by default**,
please, open a merge request to include your specs and details!

### **The status should show whether your machine has broken window dragging or not. (With `GLFW_FIXED=FALSE`!)**

- You can find the GPU name from the output of the command when doing
  `GLFW_FIXED=FALSE ./swap-interval-test`. This command should also display
  the OpenGL version as well as the driver version.
- The `lscpu | grep name` command can be used to view the CPU.
- Use the `uname -r` command to get the version of your kernel.
- The session type should be the output of `echo $XDG_SESSION_TYPE`.

| Status  | GPU, CPU                                                | OpenGL Version     | Driver Type + Version or other info | Kernel Version                | Session Type | Additional Info |
|---------|---------------------------------------------------------|--------------------|-------------------------------------|-------------------------------|--------------|-----------------|
| Broken  | NVIDIA GTX 1070, AMD Ryzen 7 1700X Eight-Core Processor | 3.3.0              | NVIDIA 515.76 (Proprietary)         | 5.19.12-arch1-1               | x11          | GNOME 42        |
| Working | Rembrandt, AMD Ryzen 7 6800U with Radeon Graphics       | 4.6 (Core Profile) | Mesa 22.3.0-devel (4.60)            | 5.19.14-201.fsync.fc36.x86_64 | wayland      | GNOME 42        |
| Working | Rembrandt, AMD Ryzen 7 6800U with Radeon Graphics       | 4.6 (Core Profile) | Mesa 22.3.0-devel (4.60)            | 5.19.14-201.fsync.fc36.x86_64 | x11          | GNOME 42        |

