May-Alias Analysis

This repository contains the implementation of an **intra-procedural, flow-sensitive MayAlias analysis** for pointer variables in C programs, as part of the *Introduction to Program Analysis and Compiler Optimization* course.

## 📋 Overview

**Alias Analysis** determines whether two pointer variables can point to the same memory location. In this assignment, the focus is on identifying *MayAlias* relationships — whether two pointers *may* refer to the same object at the end of a program.

## 🧠 Key Concepts

- **MayAlias Analysis**: Reports whether two pointers *may* refer to the same memory location.
- **Flow-Sensitive**: Considers the control flow of the program.
- **Intra-Procedural**: Analysis is restricted within a single procedure/function.

## 📁 Directory Structure

assignment-3/
├── assign/ # Contains the input C source files to be analyzed
├── output/ # Stores the alias analysis results as text files
├── alias_lib.cpp # LLVM pass implementing the MayAlias analysis
├── checker.sh # Script to verify directory structure and naming

markdown
Copy
Edit

## 🔧 Setup and Usage

1. **Clone the repository** inside your `llvm-project` directory.

2. **Build your LLVM pass** as instructed.

3. **Run the analysis**:
   - Convert the input `.cpp` files to LLVM IR.
   - Execute the alias analysis pass on the LLVM IR.
   - The output will be stored in the `output/` folder, with filenames matching the corresponding input files.

4. **Output Format**:
   Each line in the output file describes a pointer and its possible aliases:
ptr_name -> {alias1, alias2, ...}

csharp
Copy
Edit
- If no alias exists, use an empty set: `{}`.
- Maintain the order of pointer declarations from the source.

## 🧪 Example

Given:
```c
int *x = &a;
int *y = &b;
if (c > 0) {
x = &a;
y = &a;
}
Output:

rust
Copy
Edit
x -> {y}
y -> {x}
📌 Notes
Do not change file or folder names.

Only modify the designated sections in alias_lib.cpp.

Ensure your output matches naming conventions exactly.

Run checker.sh before submission — only submit if it shows "Accept".

📚 Hints
Use C++ STL containers like std::set, std::map, and std::vector.

Consider implementing Kildall’s algorithm for maintaining points-to sets.

Use LLVM’s isPointerTy() to identify pointer types.
