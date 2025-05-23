# Niagara Compute Shader Integration

## Overview

This project is an Unreal Engine 5 plugin that demonstrates compute shader integration with the Niagara particle system.
There are several different methods to achieve this, each has its own module.
I have provided an easy-to-configure boids example for each method.
I hope that this repo is a good place to start playing with UE5 graph builder.

Checkout the repo's [product page](https://aarontrotter.com/niagara-cs-integration/) for more information

Temaran has produced an awesome resource to learn how to write compute shaders and I suggest checking it out if you haven't already [UnrealEngineShaderPluginDemo](https://github.com/Temaran/UnrealEngineShaderPluginDemo)

![boidsVFX](https://github.com/user-attachments/assets/002d4802-f1b4-402e-b06f-34c67bafe741)

## Modules

### 1. ComputeRPExample

- **Overview**:
  - Demonstrates the use of Graph Builder to setup and run compute shaders through a boids example
- **Examples**:
  - Example1: Structured Buffer -> Niagara
  - Example2: Structured Buffer -> Render Target -> Niagara

### 2. ComputeRPLegacyExample

- **Overview**:
  - Demonstrates the use of manual management over the compute pipeline to setup and run compute shaders through a boids example
- **Examples**:
  - Example1: Structured Buffer -> Niagara

### 3. ComputeRPSUBExample

- **Overview**:
  - Demonstrates how to extend ComputeRPExample to pass shader data into materials by hooking into the scene buffer
- **Examples**:
  - Example1: Structured Buffer -> Material

### 4. NiagaraExample

- **Overview**:
  - A boids system implemented entirely within Niagara without compute shaders and using the Particle Attribute Reader instead.

### 5. ComputeCore

- **Overview**:
  - A shared module that provides default settings and utilities, ensuring consistency and efficiency across all modules.

## Prerequisites

- Unreal Engine 5.5.x
- C++ Project for generating binaries and intermediate folders
- Basic understanding of Unreal Engine and its plugin system.

## Usage

To integrate this plugin into your Unreal Engine project:

1. Clone the repository to your local machine.
2. Copy the plugin folder into the `Plugins` directory of your Unreal Engine project.
3. Rebuild project files from your .uproject
4. Enable the plugin within your project settings via the Plugins menu.
5. Open Maps/M_TestLab (persistent map)
6. Toggle examples by activating/deactivating sub-levels from the persistent level. You can launch each map individually, or mark them all as "always loaded" to run them all together.

![subLevels](https://github.com/user-attachments/assets/4e8d3a40-5799-4aee-802d-34d79005b3ca)

8. Enjoy. You can configure the plugin from "ST Compute Example" in Project Settings. You can even modify the values in runtime. :)

![ComputeRPExampleSettings](https://github.com/user-attachments/assets/8be4ec88-532b-4e8a-9102-6d1049769f4c)
![ComputeRPLegacyExampleSettings](https://github.com/user-attachments/assets/724374cf-53d5-4838-ab4e-ec62e23fa7c3)
![ComputeRPSUBExampleSettings](https://github.com/user-attachments/assets/f3f6f6ba-33f6-4ab2-999a-fe89ee2c78a1)
![NiagaraExampleSettings](https://github.com/user-attachments/assets/1d62a0b7-7101-4721-8458-a56da543c46a)

## Contributing

Contributions to this project are welcome. Please follow the standard GitHub workflow for submitting pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Release Notes

### v0.1.3
- BREAKING CHANGES - Renamed all files and classes and prepared the plugin to release free of charge of the Fab marketplace. There is now individual settings for each module.

### v0.1.2
- Refactor (won't be an easy upgrade if you have extended a lot of the logic from the last version as I renamed a lot of files and folders)
- Structured Buffer -> Niagara + Material

### v0.1.1
- Refactor

### v0.1.0
- Initial Release
