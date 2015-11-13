/*************************************************************************
	> File Name: 2.c
	> Author: Shoery
	> Mail: GfengWong@gamil.com 
	> Created Time: Mon 09 Nov 2015 06:45:18 PM CST
 ************************************************************************/

#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	int f1,f2;
	char buf[50];
	size_t nbyte=sizeof(buf);
	ssize_t bytes_read;
	ssize_t bytes_written;
	mode_t mode;
	mode=0644;
	f1=open("sp1.txt",O_RDWR,mode);
	f2=open("sp2.txt",O_WRONLY,mode);
	bytes_read = read(f1,buf,nbyte);
	printf("buf:%s\n", buf);
	write(f2, buf, bytes_read);
	while(bytes_read == nbyte){
		bytes_read = read(f1,buf,nbyte);
		bytes_written = write(f2,buf,nbyte);
	}

	close(f1);
	close(f2);
	
	return 0;
}
