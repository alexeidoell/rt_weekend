# RT in one weekend

Implementation of [Ray tracing in one weekend](https://raytracing.github.io)

### Things I added personally that were outside the scope of the book:

- Implemented tris, rather than quads to be closer to real world 3D graphics renderers
- Refactored to use optionals, rather than bool returns plus a out pointer
  - careful to use copy elision to maintain performance
- Multithreading
- CMake build file
