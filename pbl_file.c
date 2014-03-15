/**************************************************************************
                           LOG                                         

 PROGRAM NAME: pbl_file.c                                                  
 DESCRIPTIONS: 文件操作接口                                                
 AUTHOR      : xuke                                                  
 CREATE DATE : 2014-03-15                                                  
****************************************************************************/

#include <fcntl.h>
#include <time.h>
#include "pbl/pbl_global.h"
extern int errno;

#define PBL_MAX_DBLOG_BYTES         10000000L

/**************************************************************************
 FUNC:   int pbl_fileLock(FILE *fpFile)                                    
 PARAMS: fpFile - 文件句柄                                                 
 RETURN: 1 - 文件已经被锁                                                  
         0 - 锁成功                                                        
        -1 - 错误                                                          
 DESC:   文件加锁                                                          
****************************************************************************/
int pbl_fileLock(FILE *fpFile)
{
    int fd;
	struct flock tLock;

	fd = fileno(fpFile);

	tLock.l_type   = F_WRLCK;
	tLock.l_whence = 0;
	tLock.l_start  = 0L;
	tLock.l_len    = 0L;

	if (fcntl(fd, F_SETLK, &tLock) < 0)
	{
		if ((errno == EAGAIN) || (errno == EACCES))
		{
			return 1;
		}
		else
		{
			PBL_LOG(PBL_ERR300013_ERRORFCNTL, 0, errno, "pbl_fileLock:锁文件失败");
			return -1;
		}
	}

	return 0;
}

/**************************************************************************
 FUNC:   FILE *pbl_openLogFile(char *fileName)                                     
 PARAMS:                                                                   
 RETURN: 文件句柄                                                          
 DESC:   打开日志文件                                                
****************************************************************************/
FILE *pbl_openLogFile(char *fileName)
{
	FILE *fp;
    int  fd;
	struct stat tStat;
	char caLogFile[PBL_FILENAME_LEN];
	char caLogFileBack[PBL_FILENAME_LEN];
	int iResult;
	
    sprintf(caLogFile, "%s.log", fileName);
    sprintf(caLogFileBack, "%s.%s.log", fileName, pbl_getSysTime());

    if ((fp = fopen(caLogFile, "a+")) == NULL)
	{
		fprintf(stderr, "pbl_openLogFile:文件名[%s],[%s]\n", caLogFile,strerror(errno));
		return NULL;
	}

	while ((iResult = pbl_fileLock(fp)) != 0)
	{
		if (iResult == -1)
		{
			fprintf(stderr, "pbl_openLogFile:锁文件[%s]失败,[%s]\n", caLogFile,strerror(errno));
			return NULL;
		}
		fclose(fp);
		usleep(10000L);  /* 等待释放文件锁 */
    	if ((fp = fopen(caLogFile, "a+")) == NULL) /* 重新打开文件 */
		{
			fprintf(stderr, "pbl_openLogFile:文件名[%s],[%s]\n", caLogFile,strerror(errno));
			return NULL;
		}
	}

	fd = fileno(fp);
	if (fstat(fd, &tStat) != 0)
	{
		fprintf(stderr, "pbl_openLogFile:文件名[%s],[%s]\n", caLogFile,strerror(errno));
		return NULL;
	}
	/*fchmod(fd, tStat.st_mode|S_IWGRP); fchmod will spend evry much tps 
  */ 
	if (tStat.st_size > PBL_MAX_DBLOG_BYTES)
	{
		if (rename(caLogFile, caLogFileBack) != 0)
		{
			fclose(fp);
			fprintf(stderr,"pbl_openLogFile:原文件名[%s]:新文件名[%s],[%s]\n",caLogFile,caLogFileBack,strerror(errno));
			return NULL;
		}
		fclose(fp);
		return pbl_openLogFile(fileName);
	}

	return fp;
}
FILE *pbl_openLogFileNoLock(char *fileName)
{
	FILE *fp;
    int  fd;
	struct stat tStat;
	char caLogFile[PBL_FILENAME_LEN];
	char caLogFileBack[PBL_FILENAME_LEN];
	int iResult;
	
    sprintf(caLogFile, "%s.log", fileName);
 
	  memset (&tStat, 0x00, sizeof(tStat));
		iResult = stat (caLogFile, &tStat);
 
		if ( iResult == 0 && tStat.st_size > PBL_MAX_DBLOG_BYTES )
		{
			sprintf(caLogFileBack, "%s.%s.log", fileName, pbl_getSysTime());
			if (rename(caLogFile, caLogFileBack) != 0)
		  {
			fprintf(stderr,"pbl_openLogFileNoLock:原文件名[%s]:新文件名[%s],[%s]\n",caLogFile,caLogFileBack,strerror(errno));

		  }
		}
 
	  if ((fp = fopen(caLogFile, "a+")) == NULL)
	  {
 
		fprintf(stderr, "pbl_openLogFileNoLock:文件名[%s],[%s]\n", caLogFile,strerror(errno));
		return NULL;
	  }

	return fp;
}
