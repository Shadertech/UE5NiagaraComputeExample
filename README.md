# Niagara Compute Shader Example
**Overview**:
This project is an Unreal Engine 5 plugin that demonstrates compute shader integration with the Niagara particle system.
There are a number of different ways this can be achieved. Each way has its own module.
I hope that this repo is a good place to start playing with UE5 graph builder.

Temaran has produced an awesome resource to learn compute shaders and I suggest checking it out if you havent already [UnrealEngineShaderPluginDemo](https://github.com/Temaran/UnrealEngineShaderPluginDemo)

![boidsVFX](https://github.com/Shadertech/UE5NiagaraComputeExample/assets/1611567/ddcfbcdf-dc87-4623-8435-26ac3993bab0)

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
  - Demonstrates how to extend ComputeRPExample in order to pass shader data into materials by hooking into the scene buffer
- **Examples**:
  - Example1: Structured Buffer -> Material

### 4. NiagaraExample

- **Overview**:
  - A boids system implemented entirely within Niagara without compute shaders and using the Particle Attribute Reader instead.

### 5. ComputeCore

- **Overview**:
  - A shared module that provides default settings and utilities, ensuring consistency and efficiency across all modules.

## Prerequisites

- Unreal Engine 5.4.x
- C++ Project for generating binaries and intermediate folders
- Basic understanding of Unreal Engine and its plugin system.

## Usage

To integrate this plugin into your Unreal Engine project:

1. Clone the repository to your local machine.
2. Copy the plugin folder into the `Plugins` directory of your Unreal Engine project.
3. Rebuild project files from your .uproject
4. Enable the plugin within your project settings via the Plugins menu.
5. Open Maps/M_TestLab (persistent map)
6. Toggle examples by activating/deactivating sub levels from the persistent level. You can launch each map individually, or mark them all as "always loaded" to run them all together.
7. Configure boids parameters through Compute Example Settings inside Project Settings. Please assign the niagara systems as follows.

![boidsSettings](https://github.com/Shadertech/UE5NiagaraComputeExample/assets/1611567/45ec09b0-277f-40cd-9306-2d41c6b83065)

![subLevels](https://github.com/user-attachments/assets/132b7b29-eed2-44d5-bad8-8cb4f3095790)

8. Enjoy. You can play with the compute example settings even in runtime. :)

## Contributing

Contributions to this project are welcome. Please follow the standard GitHub workflow for submitting pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Release Notes

### v0.1.2
- Refactor (wont be an easy upgrade if you have extended a lot of the logic from the last version a I renamed a lot of files and folders)
- Structured Buffer -> Niagaga + Material

### v0.1.1
- Refactor

### v0.1.0
- Initial Release
