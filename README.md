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

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AdmPor - Port Management System - Project 2
Overview

Added new functionalities to the existing system, including enhanced process synchronization, detailed logging, and improved signal handling.
Running the Application

To execute the program, navigate to the ADMPOR directory and run:


    ./bin/admpor nome_ficheiro_argumentos.txt

Where nome_ficheiro_argumentos.txt is the name of the file containing the execution arguments. This file should be written in the following format:

|max_ops // Maximum number of operations                        |
|buffers_size // Size of buffers                                 |    
|n_clients // Number of clients                                  |
|n_intermediaries // Number of intermediaries                     |
|n_enterprises // Number of enterprises                          |    
|log_filename // Name of the log file                            |    
|statistics_filename // Name of the statistics file              |    
|alarm_time // Alarm timing interval                             |

Argument Guidelines

    Numerical Arguments: Use only positive integers.
    Textual Arguments:
        log_filename should have a .log extension.
        statistics_filename should have a .txt extension.

Building the Project

To generate the .o files and the admpor executable, use the command:

    make

To clean and rebuild the project, run:

    make clean
    make

Stopping the Application

You can stop the application using the stop command or by pressing CTRL+C. The application will handle both methods gracefully, ensuring a clean shutdown.

Log File

The log file records all commands entered in real-time, whether valid or not. Entries are written in the following format:


2023-05-19 10:56:38.412 op 0 0
2023-05-19 10:56:39.798 op 1 1
2023-05-19 10:56:42.366 op 6 7
2023-05-19 10:56:45.728 op 87 9
2023-05-19 10:56:47.929 op 34 6
2023-05-19 10:56:49.355 stop

Statistics File

The statistics file logs detailed information about all operations. Entries are written in the following format:
Processed Statistics:

Client 0 received 1 requests!
Client 1 received 1 requests!
Client 6 received 1 requests!
Intermediary 0 received 1 requests!
Intermediary 1 received 1 requests!
Intermediary 2 received 1 requests!
Enterprise 0 executed 1 requests!
Enterprise 1 executed 1 requests!
Enterprise 7 executed 1 requests!

Request Statistics:


Request: 0
Status: E
Client ID: 0
Intermediary ID: 0
Enterprise ID: 0
Request Created: 2023-05-19 10:56:38.443
Client Received: 2023-05-19 10:56:38.453
Intermediary Received: 2023-05-19 10:56:38.465
Enterprise Received: 2023-05-19 10:56:38.476
Total Time: 0.033

Request: 1
Status: E
Client ID: 1
Intermediary ID: 1
Enterprise ID: 1
Request Created: 2023-05-19 10:56:39.799
Client Received: 2023-05-19 10:56:39.807
Intermediary Received: 2023-05-19 10:56:39.817
Enterprise Received: 2023-05-19 10:56:39.828
Total Time: 0.029

Known Issues

    CLOCK_REALTIME Error: The error "identifier 'CLOCK_REALTIME' is undefined" in apptime.c is due to permission issues but does not affect program execution.
    Buffer Access Issues: When multiple clients or enterprises are present, non-deterministic semaphore unlocking might occur. This is mitigated by signaling the semaphore (produce_end) to eventually unlock the intended client or enterprise. This issue does not occur with the client_interm buffer as it is circular.
