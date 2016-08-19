#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h> // needed for memset


int main(int argc,char** argv)
{
        struct termios tio;
        struct termios stdio;
        int tty_fd;
        fd_set rdset;

        unsigned char c='D';
		int hex_zahl = 0;

        printf("Please start with %s (for example)\n",argv[1]);
        memset(&stdio,0,sizeof(stdio));
        stdio.c_iflag=0;
        stdio.c_oflag=0;
        stdio.c_cflag=0;
        stdio.c_lflag=0;
        stdio.c_cc[VMIN]=1;
        stdio.c_cc[VTIME]=0;
        tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
        tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking




        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
        tio.c_lflag=0;
        tio.c_cc[VMIN]=1;
        tio.c_cc[VTIME]=5;

        tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);      
        cfsetospeed(&tio,B57600);            // 115200 baud
        cfsetispeed(&tio,B57600);            // 115200 baud

        tcsetattr(tty_fd,TCSANOW,&tio);
		
		
		FILE *f = fopen("output.txt", "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}
		
		int gesamt[100];
		int pos = 0;
		int j = 0;
		
        while (c!='q')
        {
			if (read(tty_fd,&c,1)>0)        
			{
				//write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
				hex_zahl = (int)c;
				//printf("hex: %x int: %x \n", c, hex_zahl);
				//fflush(stdout);
				if(hex_zahl == 85)
				{
					for(j=0; j<pos; j++) 
					{
						fprintf(f, "%x ", gesamt[j]);	
					}
					fprintf(f, "\n");
					
					printf("Done\n");	
					pos = 0;
				}
			gesamt[pos] = hex_zahl;
			pos++;
			}                
		if (read(STDIN_FILENO,&c,1)>0)  
			{
				write(tty_fd,&c,1);                     // if new data is available on the console, send it to the serial port
			}
	}

	printf("Exit!");
	fclose(f);
	close(tty_fd);
}