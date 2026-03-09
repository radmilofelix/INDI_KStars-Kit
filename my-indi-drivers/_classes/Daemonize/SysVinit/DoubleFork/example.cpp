#include "Daemonise_SysVinit.h"

int main()
{

    int daemonResult;
#ifdef _DEBUG
    if( (daemonResult = Daemonize_SysVinit((char*)"obscompd", (char*)"/tmp", (char*)"obscomp-outfile", (char*)"obscomp-errfile",
                                  NULL)) != 0 )
    {
        fprintf(stderr, "error: daemonize failed\n");
        exit(EXIT_FAILURE);
    }
#else // ! _DEBUG
    if( (daemonResult = Daemonize_SysVinit((char*)"obscompd", (char*)"/tmp", NULL, NULL, NULL)) != 0 )
    {
        fprintf(stderr, "error: daemonize failed\n");
        exit(EXIT_FAILURE);
    }
#endif // _DEBUG

    ...
    ...

    while(1)
    {
        ...
        ...
    }
