#include"fileData.h"
#include <stdio.h>  
#include <stdlib.h> 

void openFile(char* pFilePath ,FILE **pFd)//打开文件
{
	*pFd = fopen(pFilePath,"rb");
	if (*pFd == NULL)
	{
		fputs("file error ", stderr);
		exit(1);
	}
}
double getFileSize(FILE *pFd)//获取文件大小
{
	double dSize;
	fseek (pFd , 0 , SEEK_END);  
	dSize = ftell (pFd);  
	rewind (pFd);
	return dSize;
}
