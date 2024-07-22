# Niagara Compute Shader Example
**Overview**:
This project is an Unreal Engine 5 plugin that demonstrates compute shader integration with the Niagara particle system.
I hope that this repo is a good place to start playing with UE5 graph builder.

Temaran has produced an awesome resource to learn compute shaders and I suggest checking it out if you havent already [UnrealEngineShaderPluginDemo](https://github.com/Temaran/UnrealEngineShaderPluginDemo)

![boidsVFX](https://github.com/Shadertech/UE5NiagaraComputeExample/assets/1611567/ddcfbcdf-dc87-4623-8435-26ac3993bab0)

## Modules

### 1. ComputeGBExample

- **Overview**:
  - Demonstrates the use of Graph Builder to setup and run compute shaders through a boids example as part of the PreRender pass
  - `GEngine->GetPreRenderDelegateEx`
- **Examples**:
  - Example1: Structured Buffer -> Niagara
  - Example2: Structured Buffer -> Render Target -> Niagara
  - Example3: Structured Buffer -> Instanced Static Meshes (WIP AND PRIVATISED)

### 2. ComputeRPExample

- **Overview**:
  - Demonstrates the use of Graph Builder to setup and run compute shaders through a boids example as part of a custom pass
  - `ENQUEUE_RENDER_COMMAND`
- **Examples**:
  - Example1: Structured Buffer -> Niagara

### 3. ComputeRPLegacyExample

- **Overview**:
  - Demonstrates the use of Graph Builder to setup and run compute shaders through a boids example as part of a custom pass through legacy methods of binding shader paramaters
  - `ENQUEUE_RENDER_COMMAND`
- **Examples**:
  - Example1: Structured Buffer -> Niagara

### 4. NiagaraExample

- **Overview**:
  - A boids system implemented entirely within Niagara without compute shaders and using the Particle Attribute Reader instead.

### 5. ShaderCore

- **Overview**:
  - A shared module that provides default shader settings and utilities, ensuring consistency and efficiency across all modules.

## Prerequisites

- Unreal Engine 5.4.3
- C++ Project for generating binaries and intermediate folders
- Basic understanding of Unreal Engine and its plugin system.

## Usage

To integrate this plugin into your Unreal Engine project:

1. Clone the repository to your local machine.
2. Copy the plugin folder into the `Plugins` directory of your Unreal Engine project.
3. Rebuild project files from your .uproject
4. Enable the plugin within your project settings via the Plugins menu.
5. Open Maps/M_TestLab (persistent map)
6. Toggle examples by activating/deactivated sub levels from the persistent level. All examples are always loaded by default.
7. Configure boids parameters through Compute Example Settings inside Project Settings. Please assign the niagara systems as follows.

![boidsSettings](https://github.com/Shadertech/UE5NiagaraComputeExample/assets/1611567/45ec09b0-277f-40cd-9306-2d41c6b83065)

![subLevels](https://github.com/user-attachments/assets/132b7b29-eed2-44d5-bad8-8cb4f3095790)

8. Enjoy. You can play with the compute example settings even in runtime. :)

## Contributing

Contributions to this project are welcome. Please follow the standard GitHub workflow for submitting pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
