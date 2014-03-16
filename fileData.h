#include<iostream>
#include<string>
using namespace std;
#pragma pack(1)

struct fileTile
{
	char fileName[255];//文件名
	double fileSize;//文件大小
	double index;//块的位置
	int tileSize;//块大小
	char tileData[1024];
};

void openFile(char* pFilePath ,FILE **pFd);//打开文件
double getFileSize(FILE *pFd);//获取文件大小
