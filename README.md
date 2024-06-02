# Environment Mapping & HDR Lighting

<img width="987" alt="bunnydiffuse" src="https://github.com/0xilc/median-cut/assets/28317283/dffe2c21-fb2c-46f5-9885-3ae3d41bd8ed">

## Introduction
This repository contains the implementation of an OpenGL program for Environment Mapping & HDR Lighting, developed for CENG469: Computer Graphics II. The main goal is to create a visually appealing scene using advanced rendering techniques.

## Objectives
The main objectives are:

1. Environment mapping to create a realistic skybox.
2. HDR tone mapping to render high dynamic range scenes on low dynamic range displays.
3. User interaction for dynamic scene manipulation.

## Specifications
1. **Scene Composition**:
   - Skybox surrounds the environment.
   - Sphere object at the world origin.
   - Camera fixed on the origin, orbitable by user input.

2. **HDR Equirectangular Projection**:
   - HDR equirectangular image read from disk.
   - Processed to produce 6 cubemap face textures.

3. **Environment Illumination**:
   - Implemented Median Cut Algorithm for Light Probe Sampling to calculate light positions and radiances.
   - Converted to 3D directional lights for OpenGL.

4. **Rendering Techniques**:
   - Disabled depth writing before rendering the skybox.
   - Scene rendered in HDR to an off-screen floating-point framebuffer texture.
   - Tone mapping used to display HDR scene on LDR monitors.

5. **Visualization Modes**:
   - **LightProbe Mode**: Directional lights for diffuse and specular illumination.
   - **Mirror Mode**: Sphere rendered as a perfect mirror.
   - **Glass Mode**: Sphere rendered as glass with refraction.
   - **Glossy Mode**: Adds mirror-like reflections on top of LightProbe mode.
   - **SpecularDisco Mode**: Debug mode with high Phong exponent specular effects.

6. **User Interaction**:
   - **Camera Control**: Orbit around the center object using the right mouse button.
   - **Exposure Adjustment**: Increase or decrease exposure with the W and S keys.
   - **Skybox Rotation**: Rotate the skybox and light positions with the A and D keys.
   - **Specular Toggle**: Toggle specular lighting with the F key.
   - **Light Count Adjustment**: Change the number of directional lights with the E and R keys.
   - **Visualization Modes**: Switch between different modes using the 1-5 keys.

## Additional References
- [Median Cut Algorithm for Light Probe Sampling](https://vgl.ict.usc.edu/Research/MedianCut/MedianCutSampling.pdf)
- 
## Extras
<img width="596" alt="median_cut_thumb" src="https://github.com/0xilc/median-cut/assets/28317283/d9456b15-f905-4812-8355-4381cf12a525">
<img width="445" alt="opener" src="https://github.com/0xilc/median-cut/assets/28317283/549e7b45-3d5d-4da3-88a3-32d38c312b99">
<img width="739" alt="BunnyMirror" src="https://github.com/0xilc/median-cut/assets/28317283/c0cee868-311c-4e62-bf0a-859788d820d9">

It looks nice.

