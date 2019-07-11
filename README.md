# message_queue
Interprocess communication between a server and clients using POSIX message queues in Linux.

"less /proc/sys/kernel/msgmax" to find the upper limit for the communication buffer
"ls /dev/mqueue/" to track the active POSIX message queues
"gcc main_2.c -lrt -o a.out" to compile (linking flag for rt library)
"chmod +x a.out" make it executable
"./a.out server" to run the server (CTRL+C to exit)
"./a.out client" to run every client
