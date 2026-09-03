# N-Body Gravity Simulation

A real-time, GPU-accelerated N-body gravitational simulation project that I've been working on in my own time. Written in C++ and OpenGL. The simulation renders thousands of bodies (stars, particles, etc.) which interact with each other through gravitational attraction. The program launches with an interactive GUI that allows the user to edit the simulation parameters before starting. 

### Simulation Parameters

- **Camera Configuration:** 2D or 3D Camera
- **Particle Render Mode:** Icosphere meshes or glowing point sprites
- **Particle Color Gradient:** Bodies are coloured on a gradient between two colors based on their current velocity
- **Physical Parameters:** 
   - Body Size
   - Numbers of Bodies
   - Total System Mass
   - Gravitation Constant G
- **Computational Method:**
  - Brute-force summation (Highest Accuracy)
  - Barnes–Hut tree approximation (Highest Performance)
- **Initial Condtions:**
  - Uniform random distributions
  - Elipitcal Galaxies ranging from class E0 to E8


## Building Requirements

- A GPU and drivers supporting **OpenGL 4.3+** (required for compute shaders)
- **GNU Make** 3.82 or later
- A C++17-compatible compiler (MSVC, GCC, or Clang)
- OpenMP (used for some CPU-side computation)

Note: I have currently only ran the program on a windows OS with a Nividia graphics card. I'm hoping to add comptability for other systems as well in the near future. 

### Dependencies

- [GLFW](https://www.glfw.org/) — window/context management and input
- [GLAD](https://glad.dav1d.de/) — OpenGL function loading
- [GLM](https://github.com/g-truc/glm) — vector/matrix math
- [Dear ImGui](https://github.com/ocornut/imgui) — the parameter-selection GUI

## Installation

### 1. Clone the repository & CD into the directory

```bash
git clone https://github.com/LucaCalbeaza/N-Body-Gravity-Simulation.git
cd N-Body-Gravity-Simulation
```

### 2. Install dependencies

**Using vcpkg:**

```bash
vcpkg install glfw3 glm imgui
```

### 3. Build with GNU Make

```bash
make
```

**To clean any build artifacts:**

```bash
make clean
```


### 4. Run the simulation

```bash
./main.exe
```

**Run the executable from the project's root directory**

## Usage

1. Launch the executable to open the GUI parameter menu
2. Configure the simulation parameters. Read the parameter tooltips for more details.
3. Click **Start Simulation** to launch the simulation. Enjoy the simulation :D
4. Camera can be move using WASD keys. Can also pan around using left click when using a 3D camera. 
5. Return to the GUI parameter menu with the Esc key 
6. Close the main window to exit.


 
