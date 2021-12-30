# lockfree-prog

## Build
* Normal build
  ```
  make
  ```

* Build for test
  ```
  make TEST=y
  ```
   * To test the function of the program such as no data race on spmc queue.

* Build for debugging
  ```
  make DEBUG=y 
  ```
   * Enable debugging messages.

## Misc
* Beautify code using astyle.
  ```
  make format
  ```
