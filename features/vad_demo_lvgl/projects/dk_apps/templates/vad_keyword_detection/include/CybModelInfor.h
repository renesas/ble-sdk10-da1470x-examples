#ifndef _CYB_MODEL_INFOR_H
#define _CYB_MODEL_INFOR_H


#define CYB_MODEL_SUCCESS                      0
#define CYB_MODEL_INVALID_PARAM                -1
#define CYB_MODEL_NOT_ENOUGH_MEMORY            -2

typedef struct _SCybModel
{
    const BYTE *pbyCyModelPack;
    BOOL bAllocateMemory;
    BOOL bWithTxt;
    BOOL bWithMapID;
    BOOL bWithTriMap;
    BOOL bWithSkipState;
    int nPackCount;
    int nGroupCount;
}   SCybModel;


#define CYBMODEL_GET_MEM_USAGE() (sizeof(SCybModel))


#ifdef __cplusplus
extern "C" {
#endif

int CybModelGetMemoryUsage();

HANDLE CybModelInit(const BYTE *pbyCyModelPack, BYTE *lpbyMem, int nMemSize, int *pnErr);

int CybModelRelease(HANDLE hCybModel);


/* Get group count of packed model buffer(with text).
	*   hCybModel(in): The handle of CybModel.
	* Return the total number of group model. */
int CybModelGetGroupCount(HANDLE hCybModel);

/* Get the memory address of base model.
	*   hCybModel(in): The handle of CybModel.
	* Return the memory address of base model. */
const BYTE* CybModelGetBase(HANDLE hCybModel);

/* Get the memory address of group model.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	* Return the memory address of group model. */
const BYTE* CybModelGetGroup(HANDLE hCybModel, int nGroupIndex);

const BYTE* CybModelGetGroupTxt(HANDLE hCybModel, int nGroupIndex, int *pnSize);

const BYTE* CybModelGetAllSkipState(HANDLE hCybModel);

const BYTE* CybModelGetSkipState(HANDLE hCybModel, int nGroupIndex);

const BYTE* CybModelGetAllMapID(HANDLE hCybModel);

const BYTE* CybModelGetMapID(HANDLE hCybModel, int nGroupIndex);

const BYTE* CybModelGetTriMap(HANDLE hCybModel);

/* Get the total command number of group model.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	* Return the total command number of group model. */
int CybModelGetCommandCount(HANDLE hCybModel, int nGroupIndex);

/* Get the command information.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	*   nCommandIndex(in): The command index(0 based) in the group.
	*   szCommand(out): The buffer for UTF8 command text, could be NULL.
	*   nCommandLen(in): The buffer length of UTF8 command text.
	*   pnMapID(out): The map ID of the command which is defined in DSMT, could be NULL.
	* Return the UTF8 command text. If it is a garbage command(reward = -100), empty string will return. */
char *CybModelGetCommandInfo(HANDLE hCybModel, int nGroupIndex, int nCommandIndex, char *lpszCommand, int nCommandLen, int *pnMapID, BYTE *pbySkipState);

const BYTE* UnpackBin(const BYTE *lpbyBin, int nPackIndex, int *pnUnpackBinSize);

#ifdef __cplusplus
}
#endif

#endif
