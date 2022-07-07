
#ifndef __DSPOTTERSDK_API_H
#define __DSPOTTERSDK_API_H

#if defined(_WIN32)
	#ifdef DSpotterDll_EXPORTS
		#define DSPDLL_API __declspec(dllexport)
	#endif
#endif

#ifndef DSPDLL_API
#define DSPDLL_API
#endif

#include "base_types.h"
#include "DSpotterSDKApi_Const.h"

#ifdef __cplusplus
extern "C"{
#endif

// lpbyCYBase(IN): The background model, contents of CYBase.mod.
// lppbyModel(IN): The command model.
// nMaxTime(IN): The maximum buffer length in number of frames for keeping the status of commands.
// lpbyMemPool(IN/OUT): Memory buffer for the recognizer.
// nMemSize(IN): Size in bytes of the memory buffer lpbyMemPool.
// lpbyPreserve(IN/OUT): Preserve param.
// nPreserve(IN): Preserve param.
// pnErr(OUT): DSPOTTER_SUCCESS indicates success, else error code. It can be NULL.
// Return: a recognizer handle or NULL
DSPDLL_API HANDLE DSpotter_Init_Multi(BYTE *lpbyCYBase, BYTE *lppbyModel[], INT nNumModel, INT nMaxTime, BYTE *lpbyMemPool, INT nMemSize, BYTE *lpbyPreserve, INT nPreserve, INT *pnErr);

// Purpose: Destroy a recognizer (free resources)
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotter_Release(HANDLE hDSpotter);

// Purpose: Reset recognizer
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotter_Reset(HANDLE hDSpotter);

// Purpose: Transfer voice samples to the recognizer for recognizing.
// hDSpotter(IN): a handle of the recognizer
// lpsSample(IN): the pointer of voice data buffer
// nNumSample(IN): the number of voice data (a unit is a short, we prefer to add 480 samples per call)
// Return: "DSPOTTER_ERR_NeedMoreSample" indicates call this function again, else call DSpotter_GetResult(...)
DSPDLL_API INT DSpotter_AddSample(HANDLE hDSpotter, SHORT *lpsSample, INT nNumSample);

// Purpose: Get recognition results.
// hDSpotter(IN): a handle of the recognizer
// Return: the command ID. It is 0 based
DSPDLL_API INT DSpotter_GetResult(HANDLE hDSpotter);
DSPDLL_API INT DSpotter_GetResultEPD(HANDLE hDSpotter, INT *pnWordDura, INT *pnEndSil, INT *pnNetworkLatency);
DSPDLL_API INT DSpotter_GetResultScore(HANDLE hDSpotter, INT *pnConfi, INT *pnSGDiff, INT *pnFIL);

// Purpose: Get Cmd energy(RMS)
// hDSpotter(IN): a handle of the recognizer
// Return: Value or error code
DSPDLL_API INT DSpotter_GetCmdEnergy(HANDLE hDSpotter);

//Model Param
DSPDLL_API INT DSpotter_GetNumWord(BYTE *lpbyModel);
DSPDLL_API INT DSpotter_GetModelSize(BYTE *lpbyModel);

// Purpose: Set fast mode
// nFastMode(IN): Default is 1. If we set to 1, it can save 5 MCPS with little recognition difference.
// Return: Value or error code
DSPDLL_API INT DSpotter_SetFastMode(int nFastMode);

DSPDLL_API INT DSpotter_GetMemoryUsage_Multi(BYTE *lpbyCYBase, BYTE *lppbyModel[], INT nNumModel, INT nMaxTime);


// Purpose: Set MapID bin to Engine
// hDSpotter(IN): a handle of the recognizer
// lpbMapID(IN): The MapID bin.
// Return: Value or error code
DSPDLL_API INT DSpotter_SetResultMapID_Sep(HANDLE hDSpotter, BYTE *lpbMapID);

// Purpose: Set Multi MapID to Engine
// hDSpotter(IN): a handle of the recognizer
// lppbMapID(IN): The MapID bin.
// nNumMapID(IN): number of MapID bins.
// Return: Value or error code
DSPDLL_API INT DSpotter_SetResultMapID_Multi(HANDLE hDSpotter, BYTE *lppbMapID[], INT nNumMapID);

// Purpose: Get mapping recognition results.
// hDSpotter(IN): a handle of the recognizer
// Return: mapping Cmd Index
DSPDLL_API INT DSpotter_GetResultMapID(HANDLE hDSpotter);

DSPDLL_API const char *DSpotter_VerInfo(void);

// Purpose: Check is any keyword is in recognition process.
// hDSpotter(IN): a handle of the recognizer
// Return: True or False
DSPDLL_API BOOL DSpotter_IsKeywordAlive(HANDLE hDSpotter);

/************************************************************************/
//  Threshold Adjust API                                                                   
/************************************************************************/
// Purpose: Set model rejection level
// hDSpotter(IN): a handle of the recognizer
// nRejectionLevel(IN): rejection level
// Return: Success or error code
// Deprecated from v2.2.7
DSPDLL_API INT DSpotter_SetRejectionLevel(HANDLE hDSpotter, INT nRejectionLevel);

// Purpose: Set model SG rejection level
// hDSpotter(IN): a handle of the recognizer
// nRejectionLevel(IN): SG rejection level
// Return: Success or error code
// Deprecated from v2.2.7
DSPDLL_API INT DSpotter_SetSgLevel(HANDLE hDSpotter, INT nRejectionLevel);

// Purpose: Set model FIL rejection level
// hDSpotter(IN): a handle of the recognizer
// nRejectionLevel(IN): FIL rejection level
// Return: Success or error code
// Deprecated from v2.2.7
DSPDLL_API INT DSpotter_SetFilLevel(HANDLE hDSpotter, INT nRejectionLevel);

// Purpose: Set engine response time
// hDSpotter(IN): a handle of the recognizer
// nResponseTime(IN): response time
// Return: Success or error code
// Deprecated from v2.2.7, please use DSpotter_SetEndSil
DSPDLL_API INT DSpotter_SetResponseTime(HANDLE hDSpotter, INT nResponseTime);

// Purpose: Set engine energy threshold
// hDSpotter(IN): a handle of the recognizer
// nEnergyTH(IN): energy threshold(RMS)
// Return: Success or error code
DSPDLL_API INT DSpotter_SetEnergyTH(HANDLE hDSpotter, INT nEnergyTH);

// Purpose: Get engine energy threshold
// hDSpotter(IN): a handle of the recognizer
// Return: energy threshold(RMS) or error code
DSPDLL_API INT DSpotter_GetEnergyTH(HANDLE hDSpotter);

// Purpose: Set Cmd response time
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// nResponseTime(IN): response time
// Return: Success or error code
// Deprecated from v2.2.7, please use DSpotter_SetCmdEndSil
DSPDLL_API INT DSpotter_SetCmdResponseTime(HANDLE hDSpotter, INT nCmdIdx, INT nResponseTime);

// Purpose: Get Cmd response time
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// Return: response time or error code
// Deprecated from v2.2.7, please use DSpotter_GetCmdEndSil
DSPDLL_API INT DSpotter_GetCmdResponseTime(HANDLE hDSpotter, INT nCmdIdx);

// Purpose: Set Cmd reward
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// nReward(IN): reward
// Return: Success or error code
// Deprecated from v2.2.7, please use DSpotter_SetCmdConfiReward
DSPDLL_API INT DSpotter_SetCmdReward(HANDLE hDSpotter, INT nCmdIdx, INT nReward);

// Purpose: Get Cmd reward
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// *pnErr(IN/Out): error code
// Return: reward
// Deprecated from v2.2.7, please use DSpotter_GetCmdConfiReward
DSPDLL_API INT DSpotter_GetCmdReward(HANDLE hDSpotter, INT nCmdIdx, INT *pnErr);

// Purpose: Set Cmd Confi. reward
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// nReward(IN): Confi. reward
// Return: Success or error code
DSPDLL_API INT DSpotter_SetCmdConfiReward(HANDLE hDSpotter, INT nCmdIdx, INT nReward);

// Purpose: Get Cmd Confi. reward
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// *pnErr(IN/Out): error code
// Return: Confi. reward
DSPDLL_API INT DSpotter_GetCmdConfiReward(HANDLE hDSpotter, INT nCmdIdx, INT *pnErr);

// Purpose: Set Group Confi. reward
// hDSpotter(IN): a handle of the recognizer
// nReward(IN): Confi. reward
// Return: Success or error code
DSPDLL_API INT DSpotter_SetConfiReward(HANDLE hDSpotter, INT nReward);

// Purpose: Get Group Confi. reward
// hDSpotter(IN): a handle of the recognizer
// *pnErr(IN/Out): error code
// Return: Confi. reward
DSPDLL_API INT DSpotter_GetConfiReward(HANDLE hDSpotter, INT *pnErr);

// Purpose: Set Group SG Diff reward
// hDSpotter(IN): a handle of the recognizer
// nReward(IN): Confi. reward
// Return: Success or error code
DSPDLL_API INT DSpotter_SetSGDiffReward(HANDLE hDSpotter, INT nReward);

// Purpose: Get Group SG Diff reward
// hDSpotter(IN): a handle of the recognizer
// *pnErr(IN/Out): error code
// Return: Confi. reward
DSPDLL_API INT DSpotter_GetSGDiffReward(HANDLE hDSpotter, INT *pnErr);

// Purpose: Set Group Ending Silence
// hDSpotter(IN): a handle of the recognizer
// nEndSil(IN): ending silence
// Return: Success or error code
DSPDLL_API INT DSpotter_SetEndSil(HANDLE hDSpotter, INT nEndSil);

// Purpose: Set Cmd Ending Silence
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// nResponseTime(IN): response time
// Return: Success or error code
DSPDLL_API INT DSpotter_SetCmdEndSil(HANDLE hDSpotter, INT nCmdIdx, INT nEndSil);

// Purpose: Get Cmd Ending Silence
// hDSpotter(IN): a handle of the recognizer
// nCmdIdx(IN): the command ID. It is 0 based
// Return: ending silence or error code
DSPDLL_API INT DSpotter_GetCmdEndSil(HANDLE hDSpotter, INT nCmdIdx);

// Purpose: Enable AGC(Auto gain control). AGC is defaultly closed. Must call this after DSpotter_Init_Multi() to activate.
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotterAGC_Enable(HANDLE hDSpotter);

// Purpose: Disable AGC(Auto gain control).
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotterAGC_Disable(HANDLE hDSpotter);

// Purpose: Set the upper bound of AGC Gain (and also the current AGC Gain)
// hDSpotter(IN): a handle of the recognizer
// fMaxGain(IN): The upper bound of AGC gain; Default is set to 32
// Return: Success or error code
// Note: 1 <= fMaxGain <= 32
DSPDLL_API INT DSpotterAGC_SetMaxGain(HANDLE hDSpotter, FLOAT fMaxGain);

// Purpose: Set threshholds to trigger AGC Gain increasing.
// hDSpotter(IN): a handle of the recognizer
// sLowerThrd(IN): AGC Gain will increasing if (current peak of frame)*(AGC Gain) < sLowerThrd; Default is set to 5000
// Return: Success or error code
// Note: 0 <= sLowerThrd <= 10000
DSPDLL_API INT DSpotterAGC_SetIncGainThrd(HANDLE hDSpotter, SHORT sLowerThrd);

// Purpose: Callback to get data after AGC.
// lpsOutputSample: Samples after AGC.
// nSampleNum: Number of samples
// lpParam: Parameters.
#if defined(_WIN32)
typedef INT (__stdcall *DSpotterAGC_GetAGCData_Callback)(SHORT *lpsOutputSample, INT nSampleNum, VOID *lpParam);
#else
typedef INT (*DSpotterAGC_GetAGCData_Callback)(SHORT *lpsOutputSample, INT nSampleNum, VOID *lpParam);
#endif

// Purpose: Set Callback to get data after AGC.
// hDSpotter(IN): a handle of the recognizer
// lpfnCallback(IN): callback
// lpParam(IN): Parameters
DSPDLL_API INT DSpotterAGC_SetCallback(HANDLE hDSpotter, DSpotterAGC_GetAGCData_Callback lpfnCallback, VOID *lpParam);

DSPDLL_API INT DSpotter_SetOptionBeginStateTime(HANDLE hDSpotter, INT nFrameNum);
DSPDLL_API INT DSpotter_SetWordOptionBeginState(HANDLE hDSpotter, INT nCmdIdx, INT nOptionBeginState);

#ifdef __cplusplus
}
#endif

#endif // __DSPOTTERSDK_API_H
