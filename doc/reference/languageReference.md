# Introduction

Aster is a general-purpose programming language designed to be safe, simple, and expressive.


## Program Entry Point

Using the `aster create <project_name>` command, the following code snippet will be generated.

```
// this is the entry point to your application
pub fn main(argc: i32, argv: **i8): i32 {
	// ..

	return 0
}
```

An `aster.yaml` file will also be created for your application.

The main entry point must exist once in any project.