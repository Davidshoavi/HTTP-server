#ifndef __REQUEST_H__

void requestHandle(int fd, int* reqCount, int* staticReqCount, int* dynamicReqCount, int threadId, struct timeval arrival, struct timeval threadCatch);

#endif
