Alloy Graphics Library
========

Alloy is yet another 2D/3D graphics library written in C++11. As mundane as that sounds, this API provides an extensive collection of UI components and engineering reference implementations of common computer vision and computer graphics algorithms for fast prototyping and visualization on windows and linux platforms.

## Contents
- Anti-aliased 2D graphics drawing (NanoVG).
- Truetype fonts (stb_truetype).
- OpenGL core 3.3 and higher support. Legacy OpenGL will not run, but you won't need it!
- Generic handling of absolute/relative coordinates in pixels, pt, dp, and mm.
- Tweenable positions, dimensions, font sizes, and colors.
- UI components: Region, Composite, Border Layout, Vertical Layout, Horizontal Layout, Scroll Bars, Draw 2D Region, Text Button, Icon Button, Text Icon Button, Text Field, Selection Box, Drop Down Box, Menu Bar, Vertical Slider, Horizontal Slider, Color Selection, File Dialog, File Field, File Button, List Box, Window Pane, Graph Plot, Progress Bar, Expand Region, Toggle Box, Check Box, Glass Pane.
- Worker, timer, and recurrent worker.
- Vector types from one to four dimensions.
- Quaternions.
- Color space support for RGBA, HSV, CieLAB, XYZ, Gray, and look-up tables.
- Sparse Matrix, Dense Matrix, Dense Vector, Array, Image, and Volume data structures.
- Data structure serialization to json, binary, and xml (cereal).
- Image IO (stb_image and tinyexr) for PNG, JPEG, TIFF, HDR, PSD, BMP, GIF, PNM, PIC, TGA, EXR, and XML (Mipav's encoding).
- Mesh IO for PLY and OBJ (tinyobj).
- Dense and sparse matrix solvers including SVD, QR, LU, CG, and BiCGstab.
- The "Any" class to enable methods with arbitrary left-hand return types.
- Perspective and orthographic cameras with methods to transform from world to screen or vice versa.
- "Awesome font" icons.
- Window screenshot (F11).
- Arbitrary mouse cursors.
- Common shaders for deferred rendering in OpenGL 3.3 and higher.
- Distance Field 2D/3D (fast-marching method).
- Point Kd-Tree 2D/3D (libkdtree).
- Approximate Nearest Neighbors in N-dimensions (nanoflann).
- Mesh Kd-Tree (ray-intersection and closest point).
- SHA1, SHA-256, SHA-384, SHA-512.
- Cross-Platform file system IO.
- NURB curves, B-Splines, and Beziers (tinyspline).
- Catmull-Clark and Loop mesh sub-division.
- Mesh primitives for box, icosahedron, sphere, cylinder, torus, plane, cone, pyramid, frustum, capsule, tessellated sphere, grid, and asteroid.

## Future Work
- Active contour segmentation
- Visual programming UI
- Anisotropic image diffusion
- Gradient Vector Flow
- KAZE features
- DAISY features
- Spring Level Sets
- Force directed graph
- One Euro Filter
- 2D physics (box2D)

## Dependencies
GLFW 3.1+ and GLEW. Source code for all other libraries is included in the repository and cross-compiles on windows and linux (unsure about mac, buy me a mac and I'll make sure it works ^_^).

## License
Alloy is <B>BSD licensed</B> and only uses libraries that also have a permissive license for commercial and academic use.


Absolute/Relative Positioning Example
-------------------------
![UnitsEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0000.png)

Composite Example
-------------------------
![CompisteEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0001.png)

Events Example
-------------------------
![EventsEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0002.png)

Drag and Drop Example
-------------------------
![DragEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0003.png)

Animation Tween Example
-------------------------
![TweenEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0004.png)

Image Example
-------------------------
![ImageEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0005.png)

Controls Example
-------------------------
![ControlsEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0006.png)

Dialog Example
-------------------------
![DialogEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0007.png)

Expand Bar Example
-------------------------
![ExpandEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0008.png)

Mesh with Matcap Shading Example
-------------------------
![MeshMatcapEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0009.png)

Mesh Wireframe Example
-------------------------
![MeshWireframeEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0010.png)

Mesh Subdivision Example
-------------------------
![MeshSubdivideEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0011.png)

Mesh Texture Example
-------------------------
![MeshTextureEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0012.png)

Mesh with Per Vertex Color Example
-------------------------
![MeshVertexColorEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0013.png)

Particles Example
-------------------------
![MeshParticleEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0014.png)

Mesh Depth / Normals / Distances Example
-------------------------
![MeshDepthEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0015.png)

Mesh Phong Shading Example
-------------------------
![MeshPhongEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0016.png)

Laplace Fill Example
-------------------------
![LaplaceFillEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0017.png)

Poisson Blend Example
-------------------------
![PoissonBlendEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0018.png)

Poisson Fill Example
-------------------------
![PoissonInpaintEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0019.png)

Image Filter Example
-------------------------
![ImageProcessingEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0020.png)

Mesh Object/Face Picker Example
-------------------------
![MeshPickerEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0021.png)

Mesh Ray Intersection Example
-------------------------
![IntersectorEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0022.png)

Mesh Smoothing Example
-------------------------
![MeshSmoothEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0023.png)

Color Space Example
-------------------------
![ColorSpaceEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0024.png)

Mesh Primitives Example
-------------------------
![MeshPrimitivesEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0025.png)

Menu System Example
-------------------------
![MenuEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0026.png)

Point Locator Example
-------------------------
![LocatorEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0027.png)

Graph Example
-------------------------
![GraphEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0028.png)

Window Pane Example
-------------------------
![WindowPaneEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0029.png)

B-Spline Example
-------------------------
![SplineEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0030.png)

Distance Field Example
-------------------------
![DistanceFieldEx](https://github.com/bclucas/Alloy-Graphics-Library/blob/master/screenshots/screenshot0031.png)
