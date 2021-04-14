![github-small](SiTronSFMLRayTracer/SiTronSFMLRayTracer/Resources/Graphics/Showcase3.png)

# Features
* Realtime software ray traced voxels
* Geometrically perfect reflections
* Render different texture regions based on face normal
* Screen door- and stochastic transparency
* Baked axis-aligned lightmaps using path tracing
* 3 types of denoisers for lightmaps: basic 3x3 kernel blur, edge avoiding a trous wavelet transform and circular gaussian kernel blur

# Running the Project
It is recommended to use Visual Studio 2017 or 2019. The project was never configured to build for x86, and therefore x64 is used.

A first-person camera can be controlled when running the project. "WASDEQ" keys will move the camera while the mouse rotates it. Left mouse button will remove a block behind the crosshair, while right mouse button places a block. The number keys 1-4 switches the currently selected block type. The variables "generateLightmaps" and "clearLightmapAfterModification" in MinecraftPlayState.h can be changed depending on if the lightmaps should be created or updated.

The world consists of a fully interactable 8x8x4 chunk where different areas can demonstrate the natural effects created by the path tracer. For example how light leaks into a small tunnel or how segments with different heights can create soft shadows. 

# Screenshots
![github-small](SiTronSFMLRayTracer/SiTronSFMLRayTracer/Resources/Graphics/Showcase1.png)
![github-small](SiTronSFMLRayTracer/SiTronSFMLRayTracer/Resources/Graphics/Showcase4.png)
![github-small](SiTronSFMLRayTracer/SiTronSFMLRayTracer/Resources/Graphics/Showcase5.png)
![github-small](SiTronSFMLRayTracer/SiTronSFMLRayTracer/Resources/Graphics/Showcase2.png)
