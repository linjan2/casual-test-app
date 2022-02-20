#include <stdio.h>
#include <ctype.h>
#include <xatmi.h>

#ifdef __cplusplus
extern "C"
{
#endif

int tpsvrinit(int argc, char **argv)
{
  printf("------[ %s ]------", __FUNCTION__);
  return 0;
}

void simpserv_toupper(TPSVCINFO *rqst)
{
    printf("------[ %s ]------", __FUNCTION__);
    for (int i = 0; i < rqst->len-1; i += 1)
    {
        rqst->data[i] = toupper(rqst->data[i]);
    }

    tpreturn(TPSUCCESS, 0, rqst->data, 0L, 0);
}

void simpserv_echo(TPSVCINFO *rqst)
{
  tpreturn(TPSUCCESS, 0, rqst->data, rqst->len, 0);
}

#ifdef __cplusplus
}
#endif
