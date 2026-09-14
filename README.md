# N-Body Gravity Simulation

A real-time, GPU-accelerated N-body gravitational simulation project that I've been working on in my own time. Written in C++ and OpenGL. The simulation renders thousands of stars which interact with each other through gravitational attraction. The program launches with an interactive GUI that allows the user to edit the simulation parameters before starting. I've integrated the open source MAGI (Many-Component Galaxy Initialiser) tool into the project, that allows the user to generate and load modern N-Body simulation initial conditions for galaxies. The MAGI generation tool is optional as it runs on Linux and thus requires a Linux environment (such as WSL) to use on windows. I've added [?] base MAGI generation files that can be loaded in as the initial condition for those who cannot run the MAGI tool, however to create new MAGI conditions the user needs to run the MAGI tool through a Linux environment as stated before.  

### Simulation Parameters

- **Camera Configuration:** 2D or 3D Camera
- **Particle Render Mode:** Icosphere meshes or glowing point sprites
- **Particle Color Gradient:** Stars are coloured on a gradient between two colors based on their current velocity
- **Physical Parameters:** 
   - Star Size
   - Numbers of Stars
   - Total System Mass
- **Computational Method:**
  - Brute-force summation (Highest Accuracy)
  - Barnes–Hut tree approximation (Highest Performance)
- **Initial Condtions:**
  - Uniform random distributions
  - Elipitcal Galaxies ranging from class E0 to E8
  - **(Optional)** MAGI Generations: Numbers of stars are locked to the number that the generation was created with and stars possess a permanent color rather than following the velocity color gradient. All other parameters remain the adjustable. MAGI conditions that can be found as ```.hdf5``` files in the ```/magiGenerations/newGenerations``` folder of the project directory. 


## Building Requirements

- A GPU and drivers supporting **OpenGL 4.3+** (required for compute shaders)
- **GNU Make** 3.82 or later
- A C++17-compatible compiler (MSVC, GCC, or Clang)
- OpenMP (used for some CPU-side computation)
- MSYS2

Note: I have currently only ran the program on a windows OS with a Nividia graphics card. I'm hoping to add comptability for other systems as well in the near future. 

### Dependencies

- [GLFW](https://www.glfw.org/) — window/context management and input
- [GLAD](https://glad.dav1d.de/) — OpenGL function loading
- [GLM](https://github.com/g-truc/glm) — vector/matrix math
- [Dear ImGui](https://github.com/ocornut/imgui) — the parameter-selection GUI

## Base Project Installation

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


## (Optional) MAGI Installation

### 1. Install WSL2 with Ubuntu

```bash
wsl --install -d Ubuntu
```

### 2. Install MAGI's build dependencies

**From within the Ubuntu/WSL Terminal:**
```bash
sudo apt update
sudo apt install -y build-essential cmake git libhdf5-dev
```

### 3. Clone & Build MAGI

**From within the Ubuntu/WSL Terminal (Ensure MAGI lives at ~/magi with its build output in ~/magi/build as the app calls it via a fixed path):**
```bash
cd ~
git clone https://bitbucket.org/ymiki/magi.git
cd magi
mkdir build && cd build
cmake .. -DUSE_GADGET_FORMAT=ON -DUSE_HDF5=ON -DUSE_SFMT=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make -j$(nproc)
```

### 4. Add HDF5 for the Windows/MinGW build

**From an MSYS2 MinGW64 shell:**
```bash
pacman -S mingw-w64-x86_64-hdf5
```

### 5. Build & Run:

```bash
make
./main.exe
```

## Usage

1. Launch the executable to open the GUI parameter menu
2. Configure the simulation parameters. Read the parameter tooltips for more details. 
3. Click **Start Simulation** to launch the simulation.
4. Camera can be move using WASD keys. Can also pan around using left click when using a 3D camera. 
5. Return to the GUI parameter menu with the Esc key 
6. Close the main window to exit.

## Future Plans
- Add more standard initial conditions such as spiral galaxies, galaxy formation, galaxy collisions, etc... 
- Implement the **Fast Multipole Method** computation algorithim method.
- Integrate **CUDA** into the project.

## References
Papers I've used during the implementation of this project: 

- Miki, Y., & Umemura, M. (2018). MAGI: many-component galaxy initializer. 
  *Monthly Notices of the Royal Astronomical Society*, 475(2), 2269–2281. 
  https://doi.org/10.1093/mnras/stx3327

- Barnes, J., & Hut, P. (1986). A hierarchical O(N log N) force-calculation 
  algorithm. *Nature*, 324(6096), 446–449. https://doi.org/10.1038/324446a0

- Karras, T. (2012). Maximizing parallelism in the construction of BVHs, 
  octrees, and k-d trees. *Proceedings of the Fourth ACM SIGGRAPH/Eurographics 
  Conference on High-Performance Graphics*, 33–37. 
  https://doi.org/10.2312/EGGH/HPG12/033-037

- Burtscher, M., & Pingali, K. (2011). An efficient CUDA implementation of 
  the tree-based Barnes Hut n-body algorithm. In W.-M. Hwu (Ed.), *GPU 
  Computing Gems Emerald Edition* (pp. 75–92). Morgan Kaufmann/Elsevier. 
  https://doi.org/10.1016/B978-0-12-384988-5.00006-1

- Plummer, H. C. (1911). On the problem of distribution in globular star 
  clusters. *Monthly Notices of the Royal Astronomical Society*, 71(5), 
  460–470. https://doi.org/10.1093/mnras/71.5.460


Website I used for learning OpenGL: 
- https://learnopengl.com/

## Third-Party Software Licenses
- This project optionally integrates with [MAGI](https://bitbucket.org/ymiki/magi)
(MIT License) for physically realistic galaxy initial conditions, and vendors.

- [HDF5](https://github.com/HDFGroup/hdf5) (BSD-style HDF5 License) for reading
simulation data. 

Full license texts are in ```LICENSES/```.





 
