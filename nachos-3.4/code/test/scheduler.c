#include "syscall.h"
void main() 
{	
	int pingPID, pongPID;
	PrintConsole("\nChuong trinh bat dau ...\n");
	pongPID = Exec("./test/pong");
	pingPID = Exec("./test/ping");
	
	while(1)
	{
	}		
}
