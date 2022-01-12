# lockfree-prog
A project to practice lock-free programming.

## How to Use
```
./spmcq -n <number of threads> -q <queue size> -d <number of data> -i <method ID>
```
  * ex. `./spmcq -n 16 -q 1024 -d 2000000 -i 0`
  * For more details, type `./spmcq -h`

## Supported Synchronization Methods
* Semaphore and mutex (method ID 1)
* Lock-free (method ID 2)

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
