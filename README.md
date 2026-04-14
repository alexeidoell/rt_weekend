# RT in one weekend

Implementation of [Ray tracing in one weekend](https://raytracing.github.io)

### Things I added personally that were outside the scope of the book:

- Implemented tris, rather than quads to be closer to real world 3D graphics renderers
- Refactored to use optionals, rather than bool returns plus a out pointer
  - careful to use copy elision to maintain performance
- Multithreading using a work stealing queue
- vectorization using highway's SIMD intrinsic wrappers
- CMake build file

#### variant branch
In the `variant` branch, you can see an alternative implementation of the bounded volume hierarchy that uses `std::variant` to store either a leaf or an internal node, rather than using inheritance and virtual functions. This could potentially have better performance due to better cache locality and reduced indirection. However due to how my bounded volume hierarchy is constructed, it requires work arounds that eliminate many benefits of the approach, making the `std::variant` approach roughly 4% slower.
