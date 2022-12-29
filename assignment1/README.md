# Assignment 1: Exploring OpenGL and Phong Lighting

## Introduction

In this assignment, you are required to create a basic model rendering with Phong Lighting program using [OpenGL](https://www.opengl.org/).

OpenGL is not really easy for beginners, and the following websites are very helpful for you to learn it: [OpenGL Tutorial](https://www.opengl-tutorial.org/), [OpenGL Tutorial from Song Ho Ana](https://www.songho.ca/opengl/), [Learn OpenGL](https://learnopengl.com/). Good Luck!

By default, [GLFW](http://www.glfw.org/) will create a window with double buffering enabled, and [GLM](https://glm.g-truc.net/0.9.8/index.html) is used to cope with math stuffs. Then, you can use the basic OpenGL calls to draw 3D objects on the window as well as manipulate the virtual camera to navigate through the virtual scene. To reduce aliasing artifacts, you can also enable anti-aliasing in OpenGL by multisampling.

In the following, we will give you the specifics about what you need to accomplish, as well as some related guidelines to assist your programming. When you have finished all your work, you will need to complete an assignment **report** (in `report` directory) to show how you completed the assignment, which needs to be written using LATEX.



## **Programming Requirements**

-  Reading mesh objects from files and draw the meshes is required. (25% **Must**)
-  Phong Lighting with shader is required. (40% **Must**)
-  Navigation with keyboard and mouse is required. (25% **Must**)

## Bonus

-  Play with light! Change light types. (20% **Optional**)
-  Using Geometry Shader. (20% **Optional**)



## Demonstration Requirements

In addition to programming, you will also need to demonstrate your code to TAs

Things you should prepare:

- Explain how you load and draw objects, show related code fragments, and demonstrate the result.
- Explain how you render 3D objects with Phong lighting model, show related code fragments, and demonstrate the result.Explain your implementation of camera navigation, show related code fragments, and demonstrate the result.

- For the optional parts, explain your implementation and show it!

Additional Notification:

- You should try your best to present your ideas as clearly as possible.
- If you do not follow the above requirements, your score will be deduced by 10% of the entire assignment score.



## Submission

You are required to submit the following things through the GitHub repository:

- Project scripts and an executable program in the Coding folder.
- A PDF-formatted report which describes what you have done in the Report folder.

Submission deadline: **22:00, Oct 7, 2022**



## Grading rules

- You can choose to do the bonus items, and if you choose to do them, you will get additional scores based on the additional work you have done. But the maximum additional score will not exceed 30% of the entire score of this assignment.
- NO CHEATING! If found, your score for the assignment is zero. You are required to work INDEPENDENTLY. We fully understand that implementations could be similar somewhere, but they cannot be identical. To avoid being evaluated inappropriately, please show your understanding of your code to TAs.

- Late submission of your assignment will be subject to score deduction based on the rule on the course webpage.



## Skeleton Project/Report Template

The skeleton program and report template will be provided once you accept the assignment link of GitHub classroom which we published in Piazza. If you accept the assignment through link properly, a repository which contains the skeleton project and report template will be created under your GitHub account. Please follow the template to prepare your report.

You should complete your assignment submission to your repository through GitHub before the deadline.



## Implementation Guide

### Git Classroom

Accept the assignment in this [link in github classroom](https://classroom.github.com/a/a8wFqhTv) to start your assignment.

### Environment Setup

To build the project, you need to first download CMake if you do not have. Then you can run command

```bash
mkdir build
cd build
cmake ..
cmake --build
```

These commands will first make a directory whose name is "build" and then jump into it. After that, it uses CMake to configure the project and builds the project.

Besides, we recommend using Visual Studio in Windows and Visual Studio Code in Linux. Both of them can build the CMake-based projects automatically (maybe with help of some plugins). If you are already an experienced developer, you can choose whatever you like.

### Creating the window program using GLFW

We have already created a blank window for you to start your drawing. You should expect the following result.

![plain window](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/plain_window.png)

## Warmup Task HelloWorld OpenGL with Shader

- Required knowledge: knowledge from lecture 2 and OpenGL Tutorial for Beginners are required

In this section we are going to rendering a **Fake Fading Colorful Cube** with **Shader** like bellow, which is your first little leap in **[OpenGL](https://www.opengl.org/)**world. (If you are an expert in OpenGL, you can just skip Warmup Task, and this task is not taken into account of the final score.)

Take it easy!

![Final Outcome of Task 0](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/task0.png)

### With Shader?

Well, though Immediate Mode is taught in class, but it is no longer been adapted in modern standards, so **Pipeline Mode** is a compulsory in our tutorial and assignment.

You must get familiar with modern OpenGL rendering technique using [VAO & VBO](https://www.falloutsoftware.com/tutorials/gl/gl3.htm#vbo_vao). This [tutorial](https://learnopengl.com/Getting-started/Hello-Triangle) offers a comprehensive illustration on how to draw a triangle in a modern OpenGL style.

After figuring out how to write a shader, do not forget to fill in the `initWithCode` function in `shader.cpp` to load shader into OpenGL.

### Fake Cube?

Actually, this is not a 3D cube, but composed of several 2D triangles. After Lecture 3, 3D model will be a piece of cake. you can use the following coordinates:

```cpp
X       Y
0.3,    0.37
-0.024, 0.5
-0.3,   0.32
0.3,    0.37
-0.3,   0.32
0.06,   0.1
0.3,    0.37
0.06,   0.1
0.252,  -0.18
0.06,   0.1
0.252,  -0.18
0.048,  -0.54
-0.3,   0.32
0.06,   0.1
-0.24,  -0.26
0.048,  -0.54
0.06,   0.1
-0.24,  -0.26
```

and colors: (or whatever color you want)

```cpp
R        G        B
0.583f,  0.771f,  0.014f
0.609f,  0.115f,  0.436f
0.327f,  0.483f,  0.844f
0.822f,  0.569f,  0.201f
0.435f,  0.602f,  0.223f
0.310f,  0.747f,  0.185f
0.597f,  0.770f,  0.761f
0.559f,  0.436f,  0.730f
0.359f,  0.583f,  0.152f
0.483f,  0.596f,  0.789f
0.559f,  0.861f,  0.639f
0.195f,  0.548f,  0.859f
0.014f,  0.184f,  0.576f
0.771f,  0.328f,  0.970f
0.406f,  0.615f,  0.116f
0.676f,  0.977f,  0.133f
0.971f,  0.572f,  0.833f
0.140f,  0.616f,  0.489f
```

### Fading?

You may wonder why we separate the shader from `C++` programs and to do something we can easily code out under Immediate Mode. This [tutorial of shader](https://learnopengl.com/Getting-started/Shaders) will break your mind and make you aware of the [creativity of shaders](https://www.bilibili.com/s/video/BV1594y1S7Xc)! To get a fading triangle, fragment interpolation is utilized in your fragment shader.

Just be creative.



If you have grasped using the basic feature of shader, you are ready for the rest of class, good luck!

If you failed to master it, you will probably have a tough time catching up in subsequent lectures, so please think twice before final decision.

## Task 1 Load Mesh from File 25%

- Required knowledge: knowledge from lecture 2 and OpenGL Tutorial for Beginners are required.

You are going to render a real 3D stuff, Excited? But Usually a 3D model is described by `.obj` [file](https://en.wikipedia.org/wiki/Wavefront_.obj_file), including vertices, normals and triangles face indices.

In this task you are required to load mesh from our simplified `.obj` file as following format:

```
# some comments
# some author statements

# vertex start
v float float float
v float float float
...
v float float float
# vertex end
# normal start
n float float float
n float float float
...
n float float float
# normal end

# face start
# f vertex_index normal_index vertex_index normal_index vertex_index normal_index
f int int int int int int
f int int int int int int
...
f int int int int int int
# face end
```

To be more specific, several lines of comments may appear in the file starting with `#` which can be ignored. Then there follows as many lines as the vertex count, where each line contains 3 floats representing the position of a vertex starting with `v`. For the next lines with the line count equal to the normal count, where each line contains 3 floats representing the normal of a vertex starting with `n`. And according to the face count, there are extra corresponding lines. Each line contains 6 integers `v_0`, `n_0`, `v_1`, `n_1`, `v_2`, `n_2` starting with `f`, where `v_0`, `n_0` indicate the vertex index and normal index of the first vertex, `v_1`, `n_1` indicate the vertex index and normal index of the second vertex and `v_2`, `n_2` indicate the vertex index and normal index of the third vertex.

Here, we give an example of a plane mesh file as below:

```
# plane
# CS171 course
# take it easy

# vertex start!
v -1.0 1.0 -1.0
v 1.0 1.0 -1.0
v 1.0 -1.0 -1.0
v -1.0 -1.0 -1.0
# vertex end.
# normal start :-)
n 0.0 0.0 1.0
# normal end
# face start :-P
f 0 0 1 0 3 0
f 1 0 2 0 3 0
# face end
```

You need to implement the function `loadDataFromFile()` in `mesh.cpp` to load all the data of the mesh from this simplified mesh file. You can organize the data with the provided vertex structure in `mesh.h` and save the vertices into the Mesh object for rendering:

```cpp
struct Vertex {
    vec3 position;
    vec3 normal;
};
```

## Task 2 Phong Lighting 30%

- Required knowledge: Knowledge from lecture 2 and OpenGL Tutorial for Beginners are required.

Until now, the drawn mesh does not appear to be 3D once it is filled with color. The reason for this is that you do not have a lighting model applied to give it the appropriate shading.

[Phong lighting model](https://learnopengl.com/Lighting/Basic-Lighting) is an approximation of the lighting in the real world, which is easy and efficient to implement and was introduced in [lecture 2](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/pdf/lecture02.pdf).

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/Phong_Lighting_model.png)

The Phong Lighting model divides lighting into three distinct components: ambient, diffuse, and specular. All of these elements are computed separately and then added together. Please see the link above for a more detailed explanation. The rendering effect will be much more exciting after implementing this light model! We show an example of the bunny object here.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/white_light_bunny.png)

## Task 3 Camera Control 25%

- Required knowledge: Knowledge from lecture 2, 3 and OpenGL Tutorial for Beginners are required

To navigate in your virtual scene, you need to change your camera from time to time.

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/MVP.png)

### Navigate in the virtual world

We only have to specify a camera position, a target position, and a vector that represents the up vector in the world space (the up vector we used for calculating the camera/view coordinate system), and the OpenGL will help you construct the view matrix by `gluLookAt(...)`.

If we want to achieve the pitch and yaw of the camera, we only need to change the target position that the camera looks at in `gluLookAt(...)`. If we want to roll the camera, we will need to change the up vector in`gluLookAt(...)`. To move the camera, we change the camera position in `gluLookAt(...)`.

### Walk around using keyboard input

As described above, we can define the camera view matrix through `gluLookAt(...)`. By changing the camera position in this matrix, camera translation can be achieved.

The camera coordinate system can be described by three vectors, as you learned in class: the up vector, the forward vector, and the right vector. To move the camera forward and backward, calculate the movement increment along the forward vector (positive or negative) based on your keyboard input, and then use this increment to update the camera position. To move the camera left and right, calculate the movement along the right vector based on your keyboard input and use this delta movement vector to update the camera position.

To respond to the keyboard input, you should define a function to process the input like below:

```cpp
void processInput(GLFWwindow *window) {
        ...
        float cameraSpeed = 0.05f; /* adjust accordingly */
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
        ...
}
```

And you may need to call this function to update the coordinate inside the render loop.

### Look around using mouse input

To be able to change the viewing angle, we need to change the camera's forward vector based on the mouse input. The yaw and pitch values are obtained from mouse (or controller/joystick) movement where horizontal mouse movement affects the yaw and vertical mouse movement affects the pitch. To capture and process mouse input, first, we have to tell GLFW that it should hide the cursor and capture it:

```cpp
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
```

Then, we should define mouse input callbacks:

```cpp
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        ...
        /* Here is how you process mouse inputs */
        ...
};
```

Then, you should register it to OpenGL:

```cpp
glfwSetCursorPosCallback(window, mouse_callback);
```

When handling mouse input for a camera, there are several steps we have to take before eventually

retrieving the direction vector:

1. Calculate the offset of the mouse at the current frame compared to the previous frame.
2. Add the mapping of the offset values to the camera's yaw and pitch values and add to them.
3. Add some constraints to the maximum/minimum yaw/pitch values.
4. Calculate the direction vector.

Check more from this [site](https://learnopengl.com/Getting-started/Camera) for OpenGL camera transformation.

## Bonus 1 Play with light! (20% max)

In fact, we have only attempted to render using a simple Phong lighting with point light source; however, there are many different types of light sources available, including multi-focus light sources, directional light sources (sunlight), spotlights, and other lighting models. These light models may not been mentioned in the course but you can be acknowledged by these sites: [site1](https://learnopengl.com/Lighting/Multiple-lights), [site2](https://www.tomdalling.com/blog/modern-opengl/08-even-more-lighting-directional-lights-spotlights-multiple-lights/).

![Spot Light model](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/spotlight.png)

Spot Light model

The score for this section is determined by the difficulty of the various lights you create.

Difficulty: Multiple Light < Directional Light < Spot Light < Carrying a Flashlight (Spot Light) with the camera.

Feel free to practice what you have learned!



A Disco Bunny!

## Bonus 2 Geometry Shader (20% max)

![Untitled](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/assignment/assignment1/geoshader.png)

The geometry shader is an optional shader stage that sits between the vertex and fragment shaders. A geometry shader receives as input a collection of vertices that together form a single primitive, such as a point or a triangle. Before sending these vertices to the next shader stage, the geometry shader can transform them as it sees fit. The geometry shader is interesting because it can convert the original primitive (set of vertices) to completely different primitives, potentially generating more vertices than were originally provided.

You can try either blow the bunny up or make fur for the bunny, it is up to you. The tutorial in [site1](https://learnopengl.com/Advanced-OpenGL/Geometry-Shader) and [site2](http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf) maybe helpful.

Looking forward to your exciting work, Bonne Chance!