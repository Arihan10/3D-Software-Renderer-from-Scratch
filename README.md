# UPDATE

Complete PHYSICS and 3D engine from scratch in C++!

Demo: https://youtu.be/L5EbMn3l9ck

Github repo:https://github.com/Arihan10/N3XTD

# 3D Software Renderer

Fully featured 3D software renderer with the ability to import .obj models! 

There is currently a C++ version and a Java version. I built both just to see the performance difference.

# Showcase

![image](https://github.com/user-attachments/assets/db738f53-249f-4adc-a7ae-9f3a2c9ba230)
90fps+ on this!

# Documentation

## How to Run

1. Place all .obj models within the root directory
2. Use object import comments at the top of import objects (plans for import GUI in future)
3. **Java only:** Use the GUI to transform objects as needed

## Importing .obj models

**Make sure you're only exported triangulated meshes.**

Example of exporting a valid .obj model from Blender:
1. Select all the objects in the scene you want to export
2. In .obj (wavefront) export settings, check "Apply All Modifiers"
3. Check "Triangulate Mesh"
4. Uncheck everything else
5. Export
6. Place .obj model inside root directory of the language version you are using (C++/Java)
7. It's ready to use!

No, I did not follow any tutorial. All code is written by me.
