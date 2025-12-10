# Lab 3 - FreeRTOS
In this lab we'll see how to compile the FreeRTOS kernel to run various application tasks on an emulated embedded system using Qemu.

## Ex 1 - Compile and execute FreeRTOS

The first step is to clone this repository using the following command:
```
git clone --recurse-submodules https://baltig.polito.it/teaching-material/labs-caos-and-os/lab3-freertos.git
```
If you clone this repository, you'll see two git files:
- `.gitignore` where are specified the types of files that must kept away from the commit, in this case all the files with the extension *.o  *.d  *.elf  *.out  *.i  *.map 
- `.gitmodules` where specified the submodule needed in this repository, that is automatically cloned within the Lab3 repo (thanks to the  --recurse-submodules flags), in this case, the FreeRTOS repository, that can be found in the subdirectory `FreeRTOS`.

In the Subdirectory `FreeRTOS` are stored all the files needed to compile our freeRTOS kernel. 

Instead, in the directory `Demo` are stored all the file for the compilation the `Makefile`, `startup.c` and the linker file `mps2_m3.ld`. Includes also the `main.c`, which we'll to modify to write our code, and `FreeRTOSConfig.h` where is stored the FreeRTOS configuration to run, we'll see how to modify this file later.

Before starting to compile, notice that inside the `Makefile` starting from line 92, inside the variable `SOURCE_FILES` are added all the kernel files that contain functions used in our Demo, and from line 98 are added all the new files created by us. We can see that for this Demo, we use `main.c` and `uart.c`. In `uart.c`, the function of our UART_printf() is defined for sending a string to the serial port of the board and the UART_init() function for initializing the uart.  

Instead on the `main.c` file, the main function simply create a task, with a priority higher than the IDLE task, that use the UART_printf function to print the message "Hello, World!" every second.

Now try to use the command of the `Makefile`:
```
make clean
make all
make gdb_start
make qemu_debug
```
To compile and debug the `Demo` on qemu.

## Ex 2 - Create Task in FreeRTOS

In this second exercise, we'll use the FreeRTOS function `xTaskCreate` to add another task to our Demo.

In particular, for this exercise, create a second task, `Task 2` with the same priority as the first task; also, this task, as the first, uses the UART_printf function to periodically write on the uart "Hello, World! from task 2" every second.
In which order are the two tasks executed, and why?

Now try to reduce the priority of the `Task2`:
```
mainTASK_PRIORITY-1
```
In this case, in which order and frequency are the two tasks executed?

Try to remove the 1-second timer in the loops from both tasks; what's happening now?

## Ex 3 - Fibonacci Sequence

In this exercise, create two tasks, both compute the Fibonacci sequence, given `N = 10000` compute the first `N` number of the sequence, according to the formula:
```
f(n) = f(n-1) + f(n-2)
```
Where the first two numbers are f(n-2 = )0 and f(n-1) = 1.
Both tasks print a message at each iteration.
What happens if both tasks have the same priority? And if they have different priorities? 

Try now to delete the task at the end of the computation using the function:
```
vTaskDelete()
```
What's happening now?

### EXTRA
Try now to run the two tasks with the same priority but with two different values of N, and let the first task that reaches the end of the computation delete the other task before deleting itself.

# Ex 4 - Sheduler

In this last exercise, we'll work with the Scheduling algorithm using different configurations.

By default, FreeRTOS uses a fixed-priority preemptive scheduling policy, with round-robin time-slicing of equal priority tasks:

- `Fixed priority` means the scheduler will not permanently change the priority of a task, although it may temporarily boost the priority of a task due to priority inheritance.

- `Preemptive` means the scheduler always runs the highest priority RTOS task that is able to run, regardless of when a task becomes able to run.  For example, if an interrupt service routine (ISR) changes the highest priority task that is able to run, the scheduler will stop the currently running lower priority task and start the higher priority task - even if that occurs within a time slice.  In this case, the lower priority task is said to have been "preempted" by the higher priority task.

- `Round-robin` means tasks that share a priority take turns entering the Running state.

- `Time sliced` means the scheduler will switch between tasks of equal priority on each tick interrupt, the time between tick interrupts being one time slice. (The tick interrupt is the periodic interrupt used by the RTOS to measure time.)

To change the scheduling algorithm behavior, you can configure the configUSE_PREEMPTION option in FreeRTOSConfig.h.
```
#define configUSE_PREEMPTION 1 
```
Where one means that the `Preemption` is activated.

Now, starting from the code used in exercise three, try to modify the configuration and notice the differences; try to use both configurations with tasks with the same priority and with tasks with different priorities.

