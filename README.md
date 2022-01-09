# lockfree-prog
A project to practice lock-free programming.

## Supported Synchronization Methods
* Semaphore and mutex

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
   * Build debug version and enable debugging messages.

## Misc
* Beautify code using astyle.
  ```
  make format
  ```

## Reference
* https://github.com/sysprog21/concurrent-programs/tree/master/spmc
* https://github.com/xhjcehust/LFTPool
