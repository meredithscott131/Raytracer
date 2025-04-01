# Assignment 5: Lights and Textures

### Team
Thomas Yi and Meredith Scott


## Working Features

### Lighting
- A light can be added to any node of the scene graph in respect to the coordinate system of the node
- properties of a light can be specified
- phong shading is incorperated
- a node has the ability to store multiple lights
- drawing a leaf now passes material properties
- lights are converted to view coordinate system

### Texturing: 
-Textures are brought in as P3 ASCII PPM files, and textures can be specified similar to instances.
-LeafNode stores textures that it uses
-ALL textures use mipmapping

Texturing Breakdown - at least 2 instances Rectangles, Spheres, and Cylinders (not textured great D: ) are textured
    - Rectangles : Ground, Walls, Roof
    - Spheres : Skin on Humanoid's shoulder (it ends up being too small to see but the texture is there)
    - Cylinders : Turret bottom half and turret top half


## Meredith's Contributions
- Everything in Working Features of Lighting
- Created Spot Light 
- Added material properties to lights.

## Thomas's Contributions
 - Integrated phong lighting onto Scenegraph
 - Everything in Working Features of Texturing


### Texture Citation:

Grass.png - Drawn in Photoshop

Brick.png - found at https://www.google.com/search?q=brick+png+texture&rlz=1C1ONGR_enUS1076US1076&oq=brick+png+texture&gs_lcrp=EgZjaHJvbWUqBwgAEAAYgAQyBwgAEAAYgAQyCAgBEAAYFhgeMggIAhAAGBYYHjIICAMQABgWGB4yCAgEEAAYFhgeMggIBRAAGBYYHjIICAYQABgWGB4yCAgHEAAYFhgeMggICBAAGBYYHjIICAkQABgWGB7SAQg0NjY0ajBqN6gCALACAA&sourceid=chrome&ie=UTF-8#imgrc=rVXFeSVqVjcacM&imgdii=YLG1izBhrzVLbM


Roof Tiles : tile.jpg - found at https://www.google.com/search?q=roof+tile+texture&sca_esv=218665eea446f0ac&rlz=1C1ONGR_enUS1076US1076&sxsrf=AHTn8zrGaEo3gwgD5zaCMD6UmaBU8Zi2Ow%3A1743118783634&ei=v-HlZ925Jr2i5NoPu-aH-Ak&ved=0ahUKEwidlZm_t6uMAxU9EVkFHTvzAZ8Q4dUDCBA&uact=5&oq=roof+tile+texture&gs_lp=Egxnd3Mtd2l6LXNlcnAiEXJvb2YgdGlsZSB0ZXh0dXJlMgsQABiABBiRAhiKBTILEAAYgAQYkQIYigUyBRAAGIAEMgoQABiABBhDGIoFMgsQABiABBiRAhiKBTILEAAYgAQYkQIYigUyBhAAGBYYHjIGEAAYFhgeMgYQABgWGB4yBhAAGBYYHkjDFVDwBljXFHADeAGQAQCYAYUBoAGtCqoBBDE1LjK4AQPIAQD4AQGYAhSgAvoKwgIKEAAYsAMY1gQYR8ICChAuGIAEGCcYigXCAgoQIxiABBgnGIoFwgIEECMYJ8ICDhAuGIAEGLEDGIMBGIoFwgIOEAAYgAQYsQMYgwEYigXCAhMQLhiABBhDGMcBGIoFGI4FGK8BwgIOEC4YgAQYsQMY0QMYxwHCAg4QLhiABBjHARiOBRivAcICDRAAGIAEGLEDGEMYigXCAggQABiABBixA8ICDRAAGIAEGLEDGBQYhwLCAgcQABiABBgKwgIQEAAYgAQYsQMYQxiDARiKBcICChAAGIAEGBQYhwKYAwCIBgGQBgiSBwQxNy4zoAezfbIHBDE0LjO4B-8K&sclient=gws-wiz-serp#vhid=b9BxPD_JybzpmM&vssid=_x-HlZ-X5CMKj5NoP-9uwyQU_56

skin.png: found at https://www.google.com/search?q=human+skin+texture&rlz=1C1ONGR_enUS1076US1076&oq=human+skin+text&gs_lcrp=EgZjaHJvbWUqDAgAEAAYFBiHAhiABDIMCAAQABgUGIcCGIAEMgYIARBFGDkyBwgCEAAYgAQyBwgDEAAYgAQyBwgEEAAYgAQyBwgFEAAYgAQyDAgGEAAYFBiHAhiABDIHCAcQABiABDIHCAgQABiABDIHCAkQABiABNIBCDMzMTVqMGo3qAIAsAIA&sourceid=chrome&ie=UTF-8#vhid=yTPliPs1tNJiXM&vssid=_DevlZ7nLFvyf5NoPw_LzyQo_36



