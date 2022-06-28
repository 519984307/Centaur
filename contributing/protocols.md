Contributing
===============

## Contributing guidelines

### Style Guidelines

- For formatting use the parent .clang-format file
- I am not against trailing return types in function declarations. use them as you like.
- For classes/structs/enums/unions use **_PascalCase_**
- For enum members use **_camelCase_**
- For global functions/variables use **_g__** prefix
- For member function use the **_m__** prefix and **_camelCase_**.
- For member functions use **_camelCase_**.
- For member static functions use **_camelCase_**.
- For function parameters use **_camelCase_**.
- For concepts use **_snake_case_** (or something extremely generic, like the standard library)
- Use as much C++20 as possible, maintaining maximum compatibility between compilers (clang/gcc/msvc)

### C++ Guidelines

- ***Never use***, specially in headers files:

```c++
 using namespace std;
```

- Unless dealing with Qt stuff, avoid using raw pointers (new/delete/malloc/free). Use **_std::unique_ptr_** and
  **_std::shared_ptr_**
- Unless you care about pointer ownership, never pass a std::shared_ptr to a function, use the raw pointer instead.
- For enum prefer to use **_enum class_** rather than the simply enum
- Unless dealing with some Qt casting avoid using **_dynamic_cast_**.
- Avoid friending stuff, I'm user there are better ways.
- Avoid the use of **_const_cast_** unless it makes sense.
- If using **_std::map_**, **_std::set_**, **_std::unordered_map_** or **_std::unordered_set_** never check the presence
  of an element using the **_operator[]_**
- See [the warnings section](warnings.md). For handling compiler warnings.
- Once, C++20 modules are well implemented, we will try to use them whenever possible.
- Use exception for error management, avoid returning error codes and never use them for program flow.
- const and constexpr thins as much as possible.
- Optimize when makes sense, otherwise, let the compiler do its thing.

### CMake

- I am not a CMake specialist, nor I like CMake, but it is the tool we have. If you have cmake sorcery that can be well
  applied to this project please do.
