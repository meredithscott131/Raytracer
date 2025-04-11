# Assignment 6: Ray Tracing

### Team
Thomas Yi and Meredith Scott

## Working Features
- 3D ray class `Ray.h`
- HitRecord class `HitRecord.h`
- Toggling between OpenGL and Raytracing modes via 'S' key in `Controller::onkey()` and `Model::toggleRenderMode()`
- Rendering of the ray caster via `RaytracerRenderer.h` and `View::raytrace()`
- Calculating of ray intersections via object classes `Box.h`, `Sphere.h`, etc.
- Writing raytraced output to a PPM image via `PPMImageWriter.h` and `View::raytrace()`

### Images Submitted
- Final rendered image can be found in src/images/raytraced_scene_final.png
    - This scene uses spheres, boxes, cones, and cylinders
    - spotlight can be seen working as part of the screen (front of the scene) is off the radius of the light, rendering it black.
- OpenGL rendering of the same scene with the same camera position is found at src/images/Final Scene OpenGL Rendering.png
- scene file used to create this scene is found at src/scenegraphmodels/final-raytrace.txt


## Extra Credit
- Raytracing supports cylinders
- Raytracing supports cones
- Final raytraced scene contains 15+ object instances and depicts a scene of a picnic in the middle of three trees. (Cylinders are used as the trunk of the trees, and cones are used as the leaves of the pine trees.)

## Thomas's Contributions
- 1.1 Laying the foundation
- 1.5 Shading via `View::shade()`
- Configured scenegraph output

## Meredith's Contributions
- 1.2 Setting up the basic ray tracing
- 1.3/1.4 Implemented `RaytraceRenderer.h`, `View::raytrace()`, and the object classes `Box.h`, `Sphere.h`, etc.
- 1.3 Basic coloring via `View::shade()`