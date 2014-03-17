#include<iostream>
#include"fileData.h"
#include<string>
#include<boost/bind.hpp>
#include <sys/socket.h> 
#include<boost/smart_ptr.hpp>
#include<boost/asio.hpp>
#include <boost/filesystem.hpp> 
#include <boost/filesystem/fstream.hpp> 

using namespace std;
using namespace boost::asio;
using ip::tcp;
using boost::system::error_code;

struct CHelloWorld_Service
{
	CHelloWorld_Service(io_service &iosev)
		:m_iosev(iosev), m_acceptor(iosev, 
 				tcp::endpoint(tcp::v4(),1000 )){}

	void start()
	{
 		//开始等待连接（非阻塞）
		boost::shared_ptr<tcp::socket> psocket(new tcp::socket(m_iosev));
		//触发的事件只有error_code参数，所以用boost::bind把socket绑定进去
		m_acceptor.async_accept(*psocket,
				boost::bind(&CHelloWorld_Service::accept_handler,this,psocket,_1));
	}
	
	
	//有客户端连接时accept_handler触发
	void accept_handler(boost::shared_ptr<tcp::socket>psocket, error_code ec)
	{
		if(ec) return;
		//等待继续连接
		start();
		//显示远程IP
		std::cout<<psocket->remote_endpoint().address()<<std::endl;

		cout<<"begin write\n";
		fileTile fT;	
		char *pBuf = (char *)malloc(sizeof(fileTile));
		memset(pBuf,0,sizeof(fileTile));

		double dWriteSize = 0.0;



		FILE *pFd = fopen(fT.fileName, "wb");
		
		boost::asio::async_read(*psocket,boost::asio::buffer((void *)pBuf, sizeof(fileTile)),
			boost::bind(&CHelloWorld_Service::read_handler,this,psocket, &pBuf, &fT, &pFd, &dWriteSize));

	}
	
	
	//异步读操作完成后read_handler触发
	void read_handler(boost::shared_ptr<tcp::socket>psocket,
			char **pBuf, fileTile *pFileTile, FILE **pFd, double *dWriteSize)
	{
		cout<<"enter read_handler"<<endl;
		memcpy(pFileTile, *pBuf, sizeof(fileTile));
		cout<<"begin fwrite\n";
		int i = fwrite(pFileTile->tileData,sizeof(char),pFileTile->tileSize,*pFd);
		if(i != pFileTile->tileSize)
		{
			cout<<"fwrite error!"<<endl;
			return ;
		}
		memset(*pBuf,0,sizeof(fileTile));
		error_code ec;
		psocket->write_some(buffer("ok"), ec);//发送接收应答，给客户端
		cout<<"send ok to windows\n";

		*dWriteSize += pFileTile->tileSize;
		if(*dWriteSize == pFileTile->fileSize)//获取全部数据，释放资源，关闭文件
		{

			free(*pBuf);
			fclose(*pFd);
		}
		else//再次启动异步读操作
		{
			boost::asio::async_read(*psocket,boost::asio::buffer((void *)pBuf, sizeof(fileTile)),
				boost::bind(&CHelloWorld_Service::read_handler,this,psocket, pBuf, pFileTile, pFd, dWriteSize));

		} 
	}
	//异步写操作完成后write_handler触发
	void write_handler(boost::shared_ptr<std::string>pstr,error_code ec,
		size_t bytes_transferred)
	{
		if(ec)
		std::cout<<"发送失败!"<<std::endl;
		else
 		std::cout<<*pstr<<"已发送"<<std::endl;
	}
	private:
	io_service &m_iosev;
	ip::tcp::acceptor m_acceptor;
};

int main()
{
	io_service iosev;
	CHelloWorld_Service sev(iosev);
	//开始等待连接
	sev.start();
	iosev.run();
 	return 0;

}
