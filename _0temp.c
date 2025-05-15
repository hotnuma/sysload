
#if 0

#define APPNAME "gpuload"

static void error_exit(const char *msg)
{
    if (!msg || *msg == '\0')
    {
        msg = "an error occurred";
    }

    printf("*** %s\nabort...\n", msg);

    exit(EXIT_FAILURE);
}

static void usage_exit()
{
    printf("*** usage :\n");
    printf("%s\n", APPNAME);
    printf("abort...\n");

    exit(EXIT_FAILURE);
}

#endif

