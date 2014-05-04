# Option type in C++

## Introduction

This is my attempt of implementing Option types in C++

Option types are very popular in functionnal programming languages like Haskell
or Scala.
An Option type is a polymorphic type that can either represent no value (None)
or some value (Some).

In functionnal programming, option types are for example used when a function
can fail or return an empty value. Haskell defines option type as follows :

```haskell
data Maybe a = Just a | Nothing
```

Scala defines it as follows :

```Scala
Option[A] = if (x == null) None else Some(x)
```

For example, when performing a hash-table or map lookup, the lookup function
would return Some(ValueType) if the key is present in the map or None if not.
That way, no null pointer or reference is involved and you can't get a
NullPointerException. Even Java 8 now has an Optional type, based on the one
provided by Guava, the google Java open source library.

A proposal has been made for C++14 to provide a std::optional type but for some
reason, it didn't make to the standard yet.
So I decided to give it a try and implement my own option type C++ with some
funny templating stuff.

## Getting started

If you want to use it, all you need to do is copy the optional.h header file
to your project source directory and guess what, that's it ! Since this is only
template, it only consists of a single header file.

As for the interface, the optional_test.cc should be straightforward as a
starting point.

## Running tests

To compile and run the tests, you just need cmake. On Unix, run the following
commands:

```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
./run_optional_tests
```
