AdmPor - Port Management System - Project 1
Overview

The AdmPor project simulates a port management system to handle cargo operations such as loading, unloading, and storage.
Developed in C using Linux and POSIX APIs, the system manages multiple processes cooperatively, including Clients, Intermediaries, and Enterprises.

Project Components:

    Main (main.c / main.h): Oversees other modules and user interactions.
    Process Management (process.c / process.h): Manages process creation and destruction.
    Memory Management (memory.c / memory.h): Handles dynamic and shared memory.
    Clients (client.c / client.h): Processes requests from the Main process.
    Intermediaries (intermediary.c / intermediary.h): Forwards requests from Clients to Enterprises.
    Enterprises (enterprise.c / enterprise.h): Completes requests and reports statistics.

The system operates via a command-line interface with options to create operations, check their status, and stop the system.
Running the Project

To run the AdmPor system, follow these instructions:

    Clone the Repository:
    git clone https://github.com/yourusername/AdmPor.git
    cd AdmPor

Build the Project:

    Ensure you have make and a C compiler installed.
    To compile the source code and generate the executable, run:

    make

Run the Application:

    Navigate to the bin directory:
    cd bin

Execute the program with the required arguments:


    ./admpor max_ops buffers_size n_clients n_intermediaries n_enterprises

    Arguments:
        max_ops: Maximum number of operations (requests) that can be created.
        buffers_size: Maximum size of the buffers.
        n_clients: Number of Client processes.
        n_intermediaries: Number of Intermediary processes (maximum 1).
        n_enterprises: Number of Enterprise processes.

Note: All arguments must be positive integers greater than 0.

Clean and Rebuild:

    To clean up old object files and the executable, run:

    bash

make clean

To rebuild the project, simply run:


        make



Limitations

    Intermediaries: Currently, only one Intermediary is supported. If multiple Intermediaries are present, they may receive and attempt to deliver the same request, demonstrating the need for process synchronization, which will be addressed in the next phase of the project
