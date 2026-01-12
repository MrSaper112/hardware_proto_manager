# hardware_proto_manager

A project for managing hardware protocols, devices, and transport layers.

## Documentation

You can find documentation in a link below: 
[Documentation Link](https://mrsaper112.github.io/hardware_proto_manager/index.html)

## Building the Project

Requirements
CMake ⩾ 3.21
C++ Compiler with C++ 20 support

### Linux

#### Standard Build Process

1. **Navigate to the project directory, change build to executable**:
   ```bash
   sudo chmod -x build.sh
   ```

2. **Build the project**:
   ```bash
   ./build.sh
   ```

3. **Run the executable**:
   ```bash
   ./hardware_proto               # Runs TestDevice with plain protocol
   ./hardware_proto led           # Runs LedController with plain protocol  

   ./hardware_proto plain test    # Runs TestDevice with plain protocol
   ./hardware_proto plain led     # Runs LedController with plain protocol
   
   ./hardware_proto shift            # Runs TestDevice with shift protocol (default 0x69)
   ./hardware_proto shift led        # Runs LedController with shift protocol (default 0x69)
   ./hardware_proto shift 0x21       # Runs TestDevice with shift protocol (custom value)
   ./hardware_proto shift 0x31 led   # Runs LedController with shift protocol (custom value)
   ```
