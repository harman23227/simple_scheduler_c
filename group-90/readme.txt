README 
https://github.com/harman23227/simple_scheduler_c
Contributions
alabhya- made the display function to show processes summaries including PID priority waittime etc,made load process to manage process states and helped in making schedule, helped in implementing semaphores, managed the signal handlers allowing controlled termination when ctrl c is pressed
harman- did the bonus to include priority based scheduling ,Managed multithreading using pthreads, creating and detaching threads for each process, helped in implementing semaphores for controlled access to shared resources

enter 'make' to run the scheduler 
make clean to clean
history to show history
add process/jobs by submit ./executable (default priority) or sumbit ./job <priority(1-4)> 
ctrl c to terminate shell and show details  and execute all process in round robin
