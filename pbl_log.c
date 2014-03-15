#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include "pbl/pbl_global.h"


static char mcSourceFile[PBL_FILENAME_LEN];
static int  mcSourceLine;

int pbl_fileLock(FILE *fpFile);
FILE *pbl_openLogFile(char *fileName);
FILE *pbl_openLogFileNoLock(char *fileName);
FILE *pbl_openFWHEADERLogFile(void)
{
	FILE *fp=NULL;
	char fileName[PBL_FILENAME_LEN];
	char dirName[PBL_FILENAME_LEN];
	char       *pcaAppHome;
	char       *pcaLogLevel;
	time_t     tTime;
  struct tm *ptmNow;
 
   
  fp=pbl_openDBLogFile(); 
  
  
  if(fp!= NULL)
	{
		 
	  	return fp;
	}
   
	if ((pcaAppHome = getenv("APPHOME")) == NULL)
  {
        return NULL;
  }
  if ((pcaLogLevel = getenv("LOGLEVEL")) == NULL)
	{
		    return NULL;
	}
	if (atoi(pcaLogLevel) !=6)
	{
			return NULL;
	}
	if ((tTime = time(NULL)) == -1)
	{
		return NULL;
	}
	if ((ptmNow = localtime(&tTime)) == NULL)
	{
		return NULL;
	}
	sprintf(dirName, "%s%s/%04d%02d%02d", pcaAppHome, PBL_LOGPATH,ptmNow -> tm_year + 1900,
	                                                 ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
 if(pbl_opendir(dirName)){
  
 	return NULL;
  }
  sprintf(fileName, "%s/trace.%s.%04d%02d%02d", dirName, gcProgramName,
			ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday);   
 /* sprintf(fileName,"%s%s/%s%s/funtrace.%04d%02d%02d.%s",pcaAppHome,PBL_LOGPATH,gcBankOrgId,PBL_DBLOGPATH,
                     ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday,gcProgramName);
 */
  fp=pbl_openLogFile(fileName);
 
	return fp;
}

void pbl_logFileLine(const char *pcaFile, int iLine)
{
	strcpy(mcSourceFile, pcaFile);
	mcSourceLine = iLine;
}

/**************************************************************************    
 FUNC:   void pbl_log(int iErrCode, int iSqlCode, int iErrno,                 
                     const char *pcaFmt, ...)                                 
 PARAMS: iErrCode - 应用的错误代码                                            
         iSqlCode - 数据库SQLCODE, 如果是0则忽略 (I)                          
         iErrno   - 操作系统的errno, 如果是0则忽略 (I)                        
         pcaFmt   - 错误信息的格式串 (I)                                      
         ...      - 错误信息的变参部分 (I)                                    
 RETURN: 无                                                                   
 DESC:   系统日志接口                                                         
****************************************************************************/
void pbl_log(int iLogLevel, int iSqlCode, int iErrno, const char *pcaFmt, ...)
{
    FILE       *fp;
	int        fd;
	struct stat tStat;
    va_list    vlVar;
    char       *pcaAppHome;
    char       *pcaLogLevel;
    char       caLogFile[PBL_FILENAME_LEN];
    char       dirName[PBL_FILENAME_LEN];
    time_t     tTime;
    struct tm *ptmNow;
    struct timeval tTimeValue;
    char       caTimeString[80];
	  int        iLevel = iLogLevel / 100000;
	  int        i_loglevel=0;
 
    if ((pcaLogLevel = getenv("LOGLEVEL")) != NULL)
	{
 
		i_loglevel=atoi(pcaLogLevel);
 
		if ((iLevel >= 4) && i_loglevel < iLevel)
		{
			return;
		}
	}
 
    if ((pcaAppHome = getenv("APPHOME")) == NULL)
    {
        return;
    }
    if ((tTime = time(NULL)) == -1)
	{
		return;
	}
	if ((ptmNow = localtime(&tTime)) == NULL)
	{
		return;
	}
 sprintf(dirName, "%s%s/%04d%02d%02d", pcaAppHome, PBL_LOGPATH,ptmNow -> tm_year + 1900,
	                                                 ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
 if(pbl_opendir(dirName)){
 
 	return ;
  }
  sprintf(caLogFile, "%s/trace.%s.%04d%02d%02d", dirName, gcProgramName,
			ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday);   
  
   	if(i_loglevel>=8)
		{
			 fp=pbl_openDBLogFile(); 
 			 if(fp== NULL)
			 {
		  	return ;
			 }
		} else {
			if(i_loglevel==0)
				{
 
					 if ((fp =pbl_openLogFileNoLock(caLogFile))== NULL)
       			{
 
        		return;
       			}
			  }
		   else
		  	{
      			if ((fp =pbl_openLogFile(caLogFile))== NULL)
       			{
       				
       		 return;
       			}
      }
    }
 
    switch (iLevel)
    {
        case 1:
 
            fprintf(fp, "INFO :");
 
            break;
        case 2:
            fprintf(fp, "FATAL:");
            break;
        case 3:
            fprintf(fp, "ERROR:");
            break;
        case 4:
            fprintf(fp, "WARN :");
            break;
        case 5:
            fprintf(fp, "DEBUG:");
            break;
        default:
            fprintf(fp, "-----:");
            break;
    }

 
    caTimeString[0]=0;
    if (gettimeofday(&tTimeValue, NULL) != -1 )
    {
    	if ((ptmNow = localtime(&(tTimeValue.tv_sec))) != NULL)
		{
		    sprintf(caTimeString, "%04d%02d%02d:%02d%02d%02d.%06d",
            ptmNow->tm_year + 1900,	ptmNow->tm_mon + 1,	ptmNow->tm_mday,
			ptmNow->tm_hour,ptmNow->tm_min,	ptmNow->tm_sec,tTimeValue.tv_usec);
		}

    }
   
    va_start(vlVar, pcaFmt);
   	if(i_loglevel==0&&iLevel==1){
   		fprintf(fp, "%s ",caTimeString);
    	}
   	else 
   		{
	   fprintf(fp, "%s:%s[%d]:%s,%d: ", caTimeString, gcProgramName, getpid(), mcSourceFile, mcSourceLine);
			}

	if (iErrno != 0)
	{
		fprintf(fp, "OS[%d,%s]:", iErrno, strerror(iErrno));
	}
	if (iSqlCode != 0)
	{
		fprintf(fp, "DB[%d,%s]:", iSqlCode, db_getErrMsg());
  	}
    vfprintf(fp, pcaFmt, vlVar);
    fprintf(fp, "\n");
    va_end(vlVar);

    fclose(fp);
}

/****************************************************************************    
   FUNC:   void pbl_batLog(const char *pcaFmt, ...)                              
   PARAMS: 																	                                  
           																	                                   
           pcaFmt   - 错误信息的格式串 (I)                                       
           ...      - 错误信息的变参部分 (I)                                     
   RETURN: 无                                                                    
   DESC:   批量专用日志接口                                                      
  ****************************************************************************/
void pbl_batLog(const char *pcaFmt, ...)
{
    FILE *fp;
    va_list    vlVar;
    char       *pcaAppHome;
    char       caLogFile[PBL_FILENAME_LEN];
    char       dirName[PBL_FILENAME_LEN];
    time_t     tTime;
    struct tm *ptmNow;
    char       caTimeString[80];

    if ((pcaAppHome = getenv("APPHOME")) == NULL)
    {
        return;
    }
    if ((tTime = time(NULL)) == -1)
	{
		return;
	}
	if ((ptmNow = localtime(&tTime)) == NULL)
	{
		return;
	}
	sprintf(dirName, "%s%s/%04d%02d%02d", pcaAppHome, PBL_LOGPATH,ptmNow -> tm_year + 1900,
	                                                 ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
 if(pbl_opendir(dirName)){
  
 	return ;
  }
  sprintf(caLogFile, "%s/trace.%s.%04d%02d%02d", dirName, gcProgramName,
			ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday);   
    if ((fp =pbl_openLogFile(caLogFile))== NULL)
		{
   		 return;
		}
/*
    if ((fp = fopen(caLogFile, "a+")) == NULL)
    {
        return;
    }
*/
    va_start(vlVar, pcaFmt);
    vfprintf(fp, pcaFmt, vlVar);
    va_end(vlVar);

    fclose(fp);
}
/**************************************************************************
 FUNC:   FILE *pbl_openDBLogFile(char *fileName)                                     
 PARAMS:                                                                   
 RETURN: 文件句柄                                                          
 DESC:   打开数据库日志文件                                                
****************************************************************************/
FILE *pbl_openDBLogFile(void)
{
	FILE *fp=NULL;
	char fileName[PBL_FILENAME_LEN];
	char dirName[PBL_FILENAME_LEN];
	char       *pcaAppHome;
	time_t     tTime;
    struct tm *ptmNow;
    
        
	if ((pcaAppHome = getenv("APPHOME")) == NULL)
    {
        return NULL;
    }
    if (db_needLog() != 1)
	{
		return NULL;
	}
    if ((tTime = time(NULL)) == -1)
	{
		return NULL;
	}
	if ((ptmNow = localtime(&tTime)) == NULL)
	{
		return NULL;
	}
	sprintf(dirName, "%s%s/%04d%02d%02d", pcaAppHome, PBL_LOGPATH,ptmNow -> tm_year + 1900,
	                                                 ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
 if(pbl_opendir(dirName)){
 
 	return ;
  }
  sprintf(fileName, "%s/dbtrace.%s.%04d%02d%02d", dirName, gcProgramName,
			ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
 
  
 
    fp=pbl_openLogFile(fileName);

	return fp;
}

void pbl_debugString(int iLogLevel,char *psBuf, int iLength)
{
    FILE       *fp;
	  int        fd;
	  struct stat tStat;
    va_list    vlVar;
    char       *pcaAppHome;
    char       *pcaLogLevel;
    char       caLogFile[PBL_FILENAME_LEN];
    char dirName[PBL_FILENAME_LEN];
    time_t     tTime;
    struct tm *ptmNow;
    struct timeval tTimeValue;
    char       caTimeString[80];
	  int        iLevel = iLogLevel / 100000;
	 
	  register int i,j=0;
	  char 	sLine[100], sTemp[6];
   
    if ((pcaLogLevel = getenv("LOGLEVEL")) != NULL)
	  {
		if ((iLevel >= 4) && (atoi(pcaLogLevel) < iLevel))
		{
			return;
		}
	  }

    if ((pcaAppHome = getenv("APPHOME")) == NULL)
    {
        return;
    }
    if ((tTime = time(NULL)) == -1)
	  {
		return;
	  }
	  if ((ptmNow = localtime(&tTime)) == NULL)
	  {
		return;
	  }
		sprintf(dirName, "%s%s/%04d%02d%02d", pcaAppHome, PBL_LOGPATH,ptmNow -> tm_year + 1900,
	                                                 ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
    if(pbl_opendir(dirName)){
 
   	return ;
    }
    
    sprintf(caLogFile, "%s/trace.%s.%04d%02d%02d", dirName, gcProgramName,
			ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday);  
    if((atoi(pcaLogLevel)>=7))
		{
			 fp=pbl_openDBLogFile(); 
 			 if(fp== NULL)
			 {
		  	return ;
			 }
		} else
    if ((fp =pbl_openLogFile(caLogFile))== NULL)
    {
        return;
    }
   
   
    caTimeString[0]=0;
    if (gettimeofday(&tTimeValue, NULL) != -1 )
    {
    	if ((ptmNow = localtime(&(tTimeValue.tv_sec))) != NULL)
		{
		    sprintf(caTimeString, "%04d%02d%02d:%02d%02d%02d.%06d",
            ptmNow->tm_year + 1900,	ptmNow->tm_mon + 1,	ptmNow->tm_mday,
			ptmNow->tm_hour,ptmNow->tm_min,	ptmNow->tm_sec,tTimeValue.tv_usec);
		}

    }
     
     
   
	  fprintf(fp, "%s:%s[%d]:%s,%d,len=%d \n",
			caTimeString,
			gcProgramName,
			getpid(),
			mcSourceFile,
			mcSourceLine,
			iLength);
	  fprintf(fp, "%80.80s\n","----------------------------------------------------------------------------"); 		
	  for	(i=0; i<iLength; i++)
		{
			/* initialize a new line */
			if (j==0)
			{
				memset ( sLine,	' ', sizeof(sLine));
				sprintf (sTemp,	"%04d:", i );
				memcpy (sLine, sTemp, 5);
				sprintf (sTemp, ":%04d", i+15 );
				memcpy (sLine+72, sTemp, 5);
			}
			
			/* output psBuf value in hex */
			sprintf( sTemp, "%02X ", (unsigned	char)psBuf[i]);
			memcpy( &sLine[j*3+5+(j>7)], sTemp, 3);
			
			/* output psBuf in ascii */
			if ( isprint (psBuf[i]))
			{
				sLine[j+55+(j>7)]=psBuf[i];
			}
			else
			{
				sLine[j+55+(j>7)]='.';
			}
			j++;
			
			/* output the line to file */
			if (j==16)
			{
				sLine[77]=0;
				fprintf(fp, "%s\n", sLine);
				j=0;
			}
	}
	
	/* last line */
	if (j)
	{
		sLine[77]=0;
		fprintf(fp, "%s\n",	sLine);
	}
	fprintf(fp, "%80.80s\n","----------------------------------------------------------------------------"); 

	fflush(fp);
	
  fclose(fp);

}

int pbl_opendir(char *curr_dir)
{
	static char    last_dir[100];
	int i_result;
	int i_dirlen=strlen(curr_dir);
  if(i_dirlen>0&&memcmp(curr_dir,last_dir,i_dirlen))
  	{
  		struct stat fileStat;
  	
	    if(stat(curr_dir, &fileStat) != 0)
	    {
  		i_result=mkdir(curr_dir,0770);
  		if(i_result==0)
  			{
  				 
  				memcpy(last_dir,curr_dir,i_dirlen);
  			}
  			else{
  				PRTRACE("%s open dir %s error %d, %s\n",gcProgramName,curr_dir,i_result,strerror(errno));
  			}
  			return i_result;
  		} 
  	 
  		memcpy(last_dir,curr_dir,i_dirlen);
  		return 0;
  	} else if (i_dirlen==0)
  		{
  				PRTRACE("%s open dir %s error %d, %s\n",gcProgramName,curr_dir,i_result,strerror(errno));
  			return -1;
  		}
  		else {
  			
  			return 0;
  		}
}
