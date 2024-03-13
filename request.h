#ifndef __REQUEST_H__

void requestHandle(int fd, int* reqCount, int* staticReqCount, int* dynamicReqCount, int threadId);

#endif
