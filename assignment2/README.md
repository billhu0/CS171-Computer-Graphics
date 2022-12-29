# Assignment 2 : Geometric Modeling

## Introduction

In this assignment, you will enter the geometric modeling section by generating Bézier surfaces and optionally, B-Spline / NURBS surfaces. To achieve surface construction, you can rely on the idea of divide and conquer, and subdivide the problem into sub-problems in the lower dimension, which is the evaluation of Bézier curve. Once you’ve created the curve, creating surface is a piece of cake.

In the following, we will give you the specifics about what you need to accomplish, as well as some related guidelines to assist your programming. When you have finished all your work, you will need to complete an assignment **report** (in `report` directory) to show how you completed the assignment, which needs to be written using LATEX.

## Note

Before doing the assignment, please read the materials on [curve and surface modeling](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/materials/Curves and Surfaces for Computer Graphics.pdf), and in particular, Bézier curves and surfaces. You can also refer to [Delaunay triangulation](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/materials/Delaunay Mesh Generation.pdf) and [subdivision methods](http://graphics.stanford.edu/courses/cs468-10-fall/LectureSlides/10_Subdivision.pdf) for more advanced implementations.

## **Programming Requirements**

-  Implementation of the basic iterative de Casteljau Bézier vertex evaluation algorithm is required. (25% **Must**)
-  Construction of Bézier Surface with normal evaluation at each mesh vertex is required. (40% **Must**)
-  Rendering a Bézier Surface in a OpenGL window based on vertex array and vertex normal is required. (5% **Must**)
-  Tea party! Stitching multiple Bézier surface patches together to create a complex meshes is required. (20% **Must**)

## Bonus

-  Adaptive mesh construction in parameter space based on the curvature estimation. (15% **Optional**)
-  B-Spline / NURBS surface construction. (20% **Optional**)
-  Interactive editing (by selection) of control points. (10% **Optional**)
-  Make a Cut! Cut the Bézier surface with a plane by regenerating the surface mesh according to the cut. (30% **Optional**)

## Demonstration Requirements

In addition to programming, you will also need to demonstrate your code to TAs.

Things you should prepare:

- Explain how you implement the basic iterative de Casteljau Bézier vertex evaluation algorithm, and show the related code fragments.
- Explain how you construct the Bézier surface and evaluate the vertex normal, and show the related code fragments.
- Explain how you render the Bézier surfaces based on the vertex array, show the related code fragments, and demonstrate the result.
- Explain how you stitch multiple Bézier surface patches together, show the related code fragments, and demonstrate the result.
- For the optional part, explain your implementation and show it!

Additional Notification:

- You should try your best to present your ideas as clearly as possible.
- If you do not follow the above requirements, your score will be deduced by 10% of the entire assignment score.

## Submission

You are required to submit the following things through the GitHub repository:

- Project scripts and an executable program in the Coding folder.
- A PDF-formatted report which describes what you have done in the Report folder.

Submission deadline: **UTC+8 22:00, Oct 28, 2022**

## Grading rules

- You can choose to do the bonus items, and if you choose to do them, you will get additional scores based on the additional work you have done. But the maximum additional score will not exceed 30% of the entire score of this assignment.

NO CHEATING! If found, your score for the assignment is zero. You are required to work INDEPENDENTLY. We fully understand that implementations could be similar somewhere, but they cannot be identical. To avoid being evaluated inappropriately, please show your understanding of your code to TAs.

- Late submission of your assignment will be subject to score deduction based on the rule on the course webpage.

## Skeleton Project/Report Template

By default, [GLFW](http://www.glfw.org/) will create a window with double buffering enabled, and [GLM](https://glm.g-truc.net/0.9.8/index.html) is used to cope with math stuffs. Then, you can use the basic OpenGL calls to draw 3D objects on the window as well as manipulate the virtual camera to navigate through the virtual scene. To reduce aliasing artifacts, you can also enable anti-aliasing in OpenGL by multisampling.

The skeleton program and report template will be provided once you accept the assignment link of GitHub classroom which we published in Piazza. If you accept the assignment through link properly, a repository which contains the skeleton project and report template will be created under your GitHub account. Please follow the template to prepare your report.

You should complete your assignment submission to your repository through GitHub before the deadline.

## Implementation Guide

### Git Classroom

Accept the assignment in this [link in github classroom](https://classroom.github.com/a/xz9QaX36) to start your assignment.

### Environment Setup

To build the project, you need to first download CMake if you do not have. Then you can run command

```bash
mkdir build
cd build
cmake ..
cmake --build / make -j $nproc
```

These commands will first make a directory whose name is "build" and then jump into it. After that, it uses CMake to configure the project and builds the project.

Besides, we recommend using Visual Studio on Windows and Visual Studio Code on Linux. Both of them can build the CMake-based projects automatically (maybe with help of some plugins). If you are already an experienced developer, you can choose whatever you like.

### Creating the window program using GLFW

We have already created a blank window for you to start your drawing. You should expect the following result.

![blank](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/blank.png)

### Rendering Case Enumeration

Seeing that you will implement several tasks, we offer you a simple way to choose

```cpp
BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface)
switch (choice) {
        case RenderCase::uniform_single_bezier_surface: {
            // do something
            break;
        }
        case RenderCase::uniform_multi_bezier_surface: {
            // do something
            break;
        }
    }
```

## Tasks

### Evaluating points on Bézier curve with De Casteljau's Algorithm

Here, you are supposed to implement the function `Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t)` in`bezier.cpp`, where you need to evaluate two things of each point: the point position and the tangent. For the evaluation of positions, you can follow [this site](https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/de-casteljau.html). As for the tangent, it is a by-product of the position evaluation, and thus there is no need to pay extra efforts on the tangent evaluation, seeing that the `Vertex` is a struct with position and normal, where you can save the tangent temporarily.

![decasteljau.gif](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/decasteljau.gif)

![convexhull.gif](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/convexhull.gif)

### Constructing the Bézier surface

Here, you are supposed to implement the function `Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>&control_points, float u, float v)` in `bezier.cpp` for evaluating the vertices on a Bézier surface.

Since you are already able to evaluate the vertices on a Bézier curve, you can definitely utilize it to make things easier. In short, given m \times n control points to evaluate the point at (u,v) , you can first construct m intermediate Bézier curves based on the corresponding n control points and evaluate m points at parameter v, and then, construct another Bézier curve based on the m points and evaluate the point at u, which is shown in the following GIF.

![beziersurface1.gif](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/beziersurface1.gif)

In this process, you can compute the position, and the tangent along one dimension. To compute the normal, you need to apply the cross product of the tangents along two dimensions, which can be obtained along with the above process.

Please refer to this tutorial for this part.

### Rendering the Bézier surfaces

In this part, you are supposed to firstly implement the function `void Object::init()`, which initializes all the related OpenGL variables like VAO, VBO and EBO, and configure the attributes. After this, you should implement the function `void Object::drawArrays()` or `void Object::drawArrays(const Shader& shader)` to support object rendering with VAO and VBO, and the function `void Object::drawElements()` or `void Object::drawElements(const Shader& shader)` to support object rendering with `VAO`, `VBO` and `EBO`.

After implementing this, you can successfully render all kinds of Bézier surfaces. Here we offer some examples.

![uniform](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/uniform.png)

### Tea party! Patching together multiple Bézier surfaces

Several Bézier curves/surfaces can be connected together to form a longer/larger curve/surface. A 0-th order (L0) continuity means that the curves/surfaces are connected only (the last and first control points of the first and second curve/surface are connected), but their tangents are not necessarily continuous. 1st order continuity (L1) means that not only the points are connected, but also their tangents (first derivative) are continuous at the joint. To ensure this, we need to make sure that the control polygon are colinear at the last control point of the first curve/surface and the first control point of the second curve/surface. For example, the following picture shows such a patching of two different Bézier surfaces, where the end of red points should be connected to the start of the blue points and the tangent directions are the same at the joints.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/patching.png)

Moreover we offer a far more complex scene including a teapot, a tea cup, a tea spoon shown as the following.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/tea.png)

To rendering the tea party scene, we offer you `.bzs` files in `assets` directory. A `.bzs` file can be expressed as following:

```bash
b p m n
i_00 i_01 ... i_0(m * n - 1)
i_10 ...
...
i_(b-1)0 ...
x_0 y_0 z_0
...
x_(p-1) y_(p-1) z_(p-1)
```

`b` represents the number of Bézier surfaces, `p` represents the number of control points, `m` and `n` represent the row and column numbers of the Bézier surface’s control points. For the next `b` lines, each line includes m \times n integers ranging from 0 to b-1indicating the index of the control points of a single Bézier surface. For the last `p` liens, each line have three float numbers specifying the position of control points. we have offered teacup, teapot and teaspoon’s Bézier surface file separately and a `tea_party.bzs` file putting them together.

You are supposed to implement the function `std::vector<BezierSurface> read(const std::string &path)` to read `bzs` files and create a Bézier surface array. 

### Adaptive mesh construction

See the picture below, using static sampling steps may cause precision loss. You can sample the curve adaptively by considering curvature of sampled points to achieve the effect on the right side. This [site](https://agg.sourceforge.net/antigrain.com/research/adaptive_bezier/index.html) may be useful.

![ordinary_vs_adaptive.png](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/ordinary_vs_adaptive.png)

And this is a visualized comparison between uniform sampling and adaptive sampling:

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/ordinary_vs_adaptive_example.png)

### Interactive editing (by selection) of control points

However Bézier / B-spline / NURBS surface are always used in 3D modeling software where you can edit model easily. You are required to achieve an interactive editing in your program, therefore, you may construct the mesh in the shader rather than in the CPU code.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/interactive.png)

### B-Spline / NURBS surfaces

We have also introduced B-Spline and NURBS briefly in the [lecture 5](http://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/pdf/lecture05.pdf). So, you can try your best to realize algorithms to construct the meshes. These site ([site 1](https://web.mit.edu/hyperbook/Patrikalakis-Maekawa-Cho/node15.html), [site 2](https://web.mit.edu/hyperbook/Patrikalakis-Maekawa-Cho/node20.html)) may be useful.

### Make a Cut! Cut the Bézier surface with a plane

After you have done adaptive mesh construction, let’s have a try to cut the Bézier surface with a plane. This operation is always happening when using a 3D modeling software, that we usually intersect two surfaces, or make a corss section of a model. In this task, you are required to cut the Bézier surface with a simple plane, or a more difficult ones like sphere or another Bézier surfaces. The following is an example of cutting the teacup with plane `z=2.5`. Any mathematical libraries with no GPL license can be used to help you calculate or solve equations. If you can accomplish this task correctly, you will earn the whole bonus points.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment2/cut.png)