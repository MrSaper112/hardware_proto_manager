# hardware_proto_manager

A C++20 CMake project for managing hardware protocols, devices, and transport layers.

## Building the Project

### Linux

#### Standard Build Process

1. **Navigate to the project directory**:
   ```bash
   cd ~/hardware_proto/hardware_proto_manager
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```
   
   For Debug build:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```

3. **Build the project**:
   ```bash
   cmake --build . -j$(nproc)
   ```

4. **Run the executable**:
   ```bash
   ./hardware_proto
   ```
