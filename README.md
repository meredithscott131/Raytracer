# Assignment 6: Ray Tracing

### Team
Thomas Yi and Meredith Scott

## Working Features
- 3D ray class `Ray.h`
- HitRecord class `HitRecord.h`
- Toggling between OpenGL and Raytracing modes via 'S' key in `Controller::onkey()` and `Model::toggleRenderMode()`
- Rendering of the ray caster via `RaytracerRenderer.h` and `View::raytrace()`
- Calculating of ray intersections via object classes `Box.h` and `Sphere.h`
- Writing raytraced output to a PPM image via `PPMImageWriter.h` and `View::raytrace()`

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
- 1.3/1.4 Implemented `RaytraceRenderer.h`, `View::raytrace()`, and the object classes `Box.h` and `Sphere.h`
- 1.3 Implemented `PPMImageWriter.h`
- 1.3 Coloring via `View::shade()`
- Wrote intersects for cylinders and cones
