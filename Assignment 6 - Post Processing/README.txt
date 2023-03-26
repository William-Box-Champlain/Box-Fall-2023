ImGui was utilized to allow the user to modify various parameters at run time, mostly regarding the color or position of lights in the scene

The "Material" box allows one to modify the ambient, diffuse, and specular color values of the objects in the scene. The material's shininess can also be modified.

The "Directional Light" box allows one to modify the color of the light, as well as the direction and intensity of the light.

The "Spot Light" box allows one to modify not just the color and directionality of the light, but it also allows the user to modify the position of the lights origin. 
The "radius" value is how far the light can shine in the scene, while the min and max angles modify the "width" of the light beam.

The "Point Lights" box just allows one to modify the point around which all the point lights orbit as well as the speed they rotate around that point. 
Like with the other lights, the user can also modify the emission radius of the lights.

For the attenuation formula, I used the "UE windowed" formula because of how closely it replecated the GL falloff while not requiring the same level of manual control to make it look good.