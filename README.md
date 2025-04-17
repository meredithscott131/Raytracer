# Assignment 7: Ray Tracing (Continued)

### Team
Thomas Yi and Meredith Scott

## Working Features
- Shadows via `View::applyLighting` and `View::calculateSpotlight`
- Reflections via `View::applyReflection`
- Texture mapping added to scenegraph parser `RaytracerRenderer.h`and `View::shade()`

### Images Submitted
- Final rendered image can be found in src/images/raytraced_scene_final.png
    - This scene uses spheres, boxes, cones, and cylinders
    - Contains lighting with shadows
    - Contains reflective red spheres
    - Contains textures on sky, leaves, and ground
    - spotlight can be seen working as part of the screen (front of the scene) is off the radius of the light, rendering it black.
- OpenGL rendering of the same scene with the same camera position is found at src/images/Final Scene OpenGL Rendering.png
- scene file used to create this scene is found at src/scenegraphmodels/final-raytrace.txt

## Extra Credit
- 2.1 Raytracing supports textures
- 2.3 Final raytraced scene contains 15+ object instances and depicts a scene of a picnic in the middle of three trees. Uses shadows, reflections for the red ornaments, and texturing for the sky/leaves/ground

## Thomas's Contributions
- 2.1 Texture Mapping
- 2.2 Refractions

## Meredith's Contributions
- 1.1 Shadows
- 1.2 Reflections
- Scene configuration

## Textures Used
- Sky - https://www.turbosquid.com/3d-models/3d-model-toonstyle-skydomeskybox-1098294
- Ground/Leaves - Custom made `textures/Grass.png`
