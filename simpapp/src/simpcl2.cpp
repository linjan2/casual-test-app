#include <stdio.h>
#include <string.h>
#include <xatmi.h>
#include <casual/buffer/string.h>

#ifdef __cplusplus
extern "C"
{
#endif

int main2(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("L%d: argc != 1", __LINE__);
        return -1;
    }

    printf("Input string is: %s\n", argv[1]);

    // if (tpinit(TPINIT*)NULL == -1)
    // {
    //     printf("L%d: %s", __LINE__, tperrnostring(tperrno));
    //     return -1;
    // }

    size_t sendlen = strlen(argv[1]);
    char *sendbuf;
    if (NULL == (sendbuf = tpalloc(CASUAL_STRING, NULL, sendlen+1)))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        // tpterm();
        return -1;
    }
    char *recvbuf;
    if (NULL == (recvbuf = tpalloc(CASUAL_STRING, NULL, sendlen+1)))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        // tpterm();
        return -1;
    }

    strncpy(sendbuf, argv[1], sendlen);
    long recvlen = 0;
    int ret = tpcall("toupper", sendbuf, sendlen, &recvbuf, &recvlen, 0);
    if (-1 == ret)
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        tpfree(sendbuf);
        tpfree(recvbuf);
        // tpterm();
        return -1;
    }

    printf("Returned string is: %s\n", recvbuf);

    tpfree(sendbuf);
    tpfree(recvbuf);
    // tpterm();
    return 0;
}

#ifdef __cplusplus
}
#endif
