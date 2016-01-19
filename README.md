lllm
====

The Low Level Lisp Machine - A tiny lisp machine with a JIT

TODO
=====
* Escape Analysis
* When does a value escape?
 * If it is stored in a ref
  * If it is consed to a list
  * If it is captured by a lambda
  * If it is passed to a function and that function does one of the other three things to that parameter
+ check if var gets captured is put into anlyzer capture analyis. *DONE*
* check if param escapes function is stored in function
* builtins have info for their params (solves ref/cons problem) *PARTIALLY DONE*
