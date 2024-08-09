# A Local Search Engine in C++

A multiThreading local search engine built using C++ & [`TF-IDF Algorithm`](https://en.wikipedia.org/wiki/Tf%E2%80%93idf)

## Quick Start

```console
$ make && ./se

Usage: se COMMAND <Args...>
Commands:
  index  <input_file> <output_file> Index the input file and save to the output file.
  search <index_file>               Perform a search using an indexed file.
```

## Dependencies

- `C++17 standard library`
- [`libxml2`](https://gitlab.gnome.org/GNOME/libxml2): XML toolkit implemented in C, used to parse & write xml files
- [`Snowball`](https://snowballstem.org/): Used to stem tokens

## Preformance

On my machine using `Intel i5-8250U (8) @ 3.400GHz`, I tested the application using more than 4000 document from [CppReferences files](https://en.cppreference.com/w/Cppreference:Archives), and the results are:

- Indexing:

  ```console
  $ time ./se index documents documents.out.xml
  real 0m3.311s
  user 0m12.814s
  sys  0m1.644s
  ```

- Searching:

  ```console
  $ ./se search documents.out.xml

  > add element to a vector
  2882 result found in 32ms
  ...
  ```
