#include <stdio.h>
#include <string.h>
#include <xatmi.h>
#include <casual/buffer/string.h>

#ifdef __cplusplus
extern "C"
{
#endif

int main(int argc, char **argv)
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

    char *sendbuf;
    long sendlen = 0;
    if (NULL == (sendbuf = tpalloc(CASUAL_STRING, NULL, sendlen)))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        // tpterm();
        return -1;
    }
    char *recvbuf;
    if (NULL == (recvbuf = tpalloc(CASUAL_STRING, NULL, sendlen)))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        // tpterm();
        return -1;
    }

    if (0 != casual_string_set(&sendbuf, argv[1]))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        return -1;
    }
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

    const char* value = "";
    if (0 != casual_string_get(recvbuf, &value))
    {
        printf("L%d: %s", __LINE__, tperrnostring(tperrno));
        tpfree(sendbuf);
        tpfree(recvbuf);
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
