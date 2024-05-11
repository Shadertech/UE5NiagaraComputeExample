# Niagara Compute Shader Example

This project is an Unreal Engine 5 plugin that demonstrates advanced compute shader functionalities within the Niagara particle system. It is designed to showcase the integration of compute shaders into Unreal Engine, focusing on performance optimization, real-time rendering, and complex particle simulations.

## Modules

### 1. ComputeGBExample

- **Overview**: Demonstrates the use of the Graph Builder (GB) to integrate compute shaders into Unreal Engine, showcasing a practical application of compute shaders for performance-critical tasks.
- **Key Features**:
  - Utilization of `GEngine->GetPreRenderDelegateEx` for compute shader execution, enabling dynamic shader execution based on runtime conditions.
  - Exploration of compute shader optimization techniques for better performance and resource management.

### 2. ComputeRPExample

- **Overview**: Focuses on the integration of compute shaders using a different approach, specifically through the use of `ENQUEUE_RENDER_COMMAND` within the tick function.
- **Key Features**:
  - Detailed explanation of how to enqueue render commands for compute shaders, allowing for efficient execution during the rendering pipeline.
  - Discussion on the benefits and trade-offs of this method compared to the Graph Builder approach.

### 3. NiagaraExample

- **Overview**: Presents a boids system implemented entirely within Niagara, leveraging the power of compute shaders for real-time particle simulations.
- **Key Features**:
  - Comprehensive guide on creating complex particle systems using Niagara and compute shaders, including performance considerations and best practices.
  - Exploration of Niagara's capabilities for simulating natural phenomena like flocking behavior.

### 4. ShaderCore

- **Overview**: A shared module that provides default shader settings and utilities, ensuring consistency and efficiency across all modules.
- **Key Features**:
  - Centralized shader management, including common shader functions and settings, to streamline development and maintain code quality.
  - Documentation on shader compilation and optimization techniques for better performance in Unreal Engine.

## Unreal Engine Version

This project is compatible with Unreal Engine 5.3 

## Prerequisites

- Unreal Engine 5.3 installed.
- C++ Project for generating binaries and intermediate folders
- Basic understanding of Unreal Engine and its plugin system.

## Usage

To integrate this plugin into your Unreal Engine project:

1. Clone the repository to your local machine.
2. Copy the plugin folder into the `Plugins` directory of your Unreal Engine project.
3. Enable the plugin within your project settings via the Plugins menu.

## Contributing

Contributions to this project are welcome. Please follow the standard GitHub workflow for submitting pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
