#include <iostream>   // C++ standard I/O stream library
#include <unistd.h>   // For pipe, read, write, close
#include <sys/types.h> // For pid_t
#include <sys/wait.h> // For wait
#include <cstdlib>    // For exit

class optPipe
{
    private:
        int pipefd[2];
        pid_t cpid;
        char buffer[100];

    public:
        optPipe();
        ~optPipe();

};