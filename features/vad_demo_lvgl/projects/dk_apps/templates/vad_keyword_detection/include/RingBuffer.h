#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "base_types.h"

#define RING_BUFFER_ADVANCE_FUNCTION

#define RING_BUFFER_SUCCESS						(0)
#define RING_BUFFER_ERR_OUT_OF_MEMORY			(-1)
#define RING_BUFFER_ERR_ILLEGAL_PARAM			(-2)
#define RING_BUFFER_ERR_NO_ENOUGH_DATA			(-3)
#define RING_BUFFER_ERR_PUT_DISABLE				(-4)
#define RING_BUFFER_ERR_PUT_NEED_DISABLE		(-5)

typedef struct tagRingBuffer
{
	BYTE *lpBuffer;  // Memory buffer.
	int nBufferSize; // The size of lpBuffer.
	int nStart;      // The start index of data.
	int nEnd;        // The end index of data.
	int nDataSize;   // The size of data. If nEnd > nStart, nCount = nEnd - nStart;
	BOOL bUseMalloc; // TRUE if lpBuffer us malloc(), FALSE if use outside memory.
	BOOL bEnablePut; // Enable/Disable the put function.
	BOOL bOverEnd;   // The enqueue data size is large than nBufferSize;
} RingBuffer;

#define RING_BUFFER_GET_MEM_USAGE(nRingBufferSize) (sizeof(RingBuffer) + sizeof(int) + nRingBufferSize + sizeof(int))


#ifdef __cplusplus
extern "C" {
#endif

/* Return the memory usage of ring buffer.
 *   nRingBufferSize(in): The data size of ring buffer.
 * Return the memory usage in bytes*/
int RingBufferGetMemUsage(const int nRingBufferSize);

/* Return the memory usage of ring buffer.
 *   lpMem(in): The outside memory. Ring buffer will use malloc() if it is NULL.
 *   nMemSize: The memory size of lpMem.
 *   nRingBufferSize(in): The data size of ring buffer.
 *   lphRingBuffer(out): The handle of ring buffer.
 * Return the memory usage in bytes*/
int RingBufferInit(void *lpMem, int nMemSize, int nRingBufferSize, HANDLE *lphRingBuffer);

/* Release the ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return RING_BUFFER_SUCCESS or other error value. */
int RingBufferRelease(HANDLE hRingBuffer);

/* Reset ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return RING_BUFFER_SUCCESS or other error value. */
int RingBufferReset(HANDLE hRingBuffer);

/* Check ring buffer is empty.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return 1 for TRUE, 0 for FALSE, or other error value. */
int RingBufferIsEmpty(HANDLE hRingBuffer);

/* Check ring buffer is full.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return 1 for TRUE, 0 for FALSE, or other error value. */
int RingBufferIsFull(HANDLE hRingBuffer);

/* Put data into the ring buffer, the ring buffer's data size increased.
 *   hRingBuffer(in): The handle of ring buffer.
 *   lpData(in): The data pointer, ring buffer will copy it to internal buffer.
 *   nDataSize(in): The size of lpData.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the free buffer size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferPutData(HANDLE hRingBuffer, const void *lpData, int nDataSize);

/* Get data from ring buffer, the ring buffer's data size decreased.
 *   hRingBuffer(in): The handle of ring buffer.
 *   lpData(in): The data pointer, ring buffer will copy internal buffer to it.
 *   nDataSize(in): The size of lpData.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the data size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferGetData(HANDLE hRingBuffer, void *lpData, int nDataSize);

/* To disable the put function the ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return RING_BUFFER_SUCCESS, or other error value. */
int RingBufferPutDisable(HANDLE hRingBuffer);

/* To enable the put function the ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return RING_BUFFER_SUCCESS, or other error value. */
int RingBufferPutEnable(HANDLE hRingBuffer);

/* Get the available free size of ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return the free size of ring buffer(>= 0) or other negative error value. */
int RingBufferGetFreeSize(HANDLE hRingBuffer);

/* Get the data size of ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return the data size of ring buffer(>= 0) or other negative error value. */
int RingBufferGetDataSize(HANDLE hRingBuffer);

#ifdef RING_BUFFER_ADVANCE_FUNCTION

/* Get free buffer pointer of ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nFreeSize(in): The request free size.
 *   lppFree1(out): The pointer of first free buffer.
 *   pnFree1Size(out): The size of first free buffer.
 *   lppFree2(out): The pointer of second free buffer.
 *   pnFree2Size(out): The size of second free buffer.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the free size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferGetFreeBuffer(HANDLE hRingBuffer, int nRequestFreeSize, void **lppFree1, int *pnFree1Size, void **lppFree2, int *pnFree2Size);

/* Enqueue data, reduce the free size of the ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nDataSize(in): The size of enqueue amount.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the free buffer size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferEnqueueData(HANDLE hRingBuffer, int nDataSize);

/* Get data buffer pointer of ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nRequestDataSize(in): The request data size.
 *   lppData1(out): The pointer of first data.
 *   pnData1Size(out): The size of first data.
 *   lppData2(out): The pointer of second data.
 *   pnData2Size(out): The size of second data.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the data size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferGetDataBuffer(HANDLE hRingBuffer, int nRequestDataSize, void **lppData1, int *pnData1Size, void **lppData2, int *pnData2Size);

/* Dequeue data, reduce the data size of the ring buffer.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nDataSize(in): The size of dequeue amount.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the data size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferDequeueData(HANDLE hRingBuffer, int nDataSize);

/* Get the old data size of the ring buffer. To use this API, user must call RingBufferPutDisable() at first.
 *   hRingBuffer(in): The handle of ring buffer.
 * Return the old data size of ring buffer, or other negative error value. */
int RingBufferGetOldDataSize(HANDLE hRingBuffer);

/* Get the old data of the ring buffer. To use this API, user must call RingBufferPutDisable() at first.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nOldDataOffset(in):
 *   lpBuffer(out): The buffer to store old data.
 *   nBufferSize(in): The buffer size.
 * Return the real old data size that copy to the buffer, or other negative error value. */
int RingBufferGetOldData(HANDLE hRingBuffer, int nOldDataOffset, void *lpBuffer, int nBufferSize);

/* Get old data buffer pointer of ring buffer. To use this API, user must call RingBufferPutDisable() at first.
 *   hRingBuffer(in): The handle of ring buffer.
 *   lppData1(out): The pointer of first old data.
 *   pnData1Size(out): The size of first old data.
 *   lppData2(out): The pointer of second old data.
 *   pnData2Size(out): The size of second old data.
 * Return RING_BUFFER_SUCCESS or other error value. */
int RingBufferGetOldDataBuffer(HANDLE hRingBuffer, int nOldDataOffset, void **lppOldData1, int *pnOldData1Size, void **lppOldData2, int *pnOldData2Size);

/* Get the value of DataBuffer[nIndex], the ring buffer's data size won't change.
 *   hRingBuffer(in): The handle of ring buffer.
 *   nIndex(in): The offset of data buffer, it must less than RingBufferGetDataSize().
 * Return the value of DataBuffer[nIndex]. */
BYTE RingBufferGetDataValue(HANDLE hRingBuffer, int nIndex);

/* Copy data from ring buffer, the ring buffer's data size won't change.
 *   hRingBuffer(in): The handle of ring buffer.
 *   lpData(in): The data pointer, ring buffer will copy internal buffer to it.
 *   nDataSize(in): The size of lpData.
 * Return RING_BUFFER_SUCCESS,
 *        RING_BUFFER_ERR_NO_ENOUGH_DATA if the data size of ring buffer less then nDataSize,
 *        or other error value. */
int RingBufferCopyDataValue(HANDLE hRingBuffer, void *lpData, int nDataSize);


#endif //RING_BUFFER_ADVANCE_FUNCTION


#ifdef __cplusplus
}
#endif

#endif
