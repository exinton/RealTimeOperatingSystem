//============================================================================
// Name        : WindowsTrain.cpp
// Author      : Xin Tong
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <windows.h>
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>



using namespace std;
int serial_port_open(void);
int serial_port_write(char *write_buffer);
HANDLE serial_port;
int serial_port_close(void);
void parseCommand(int argv,char *args[]);


int serial_port_close(void){
	fprintf(stderr, "Closing serial port...");
		if (CloseHandle(serial_port) == 0)
		{
			fprintf(stderr, "Error\n");
			return 1;
		}
		fprintf(stderr, "OK\n");

}




int serial_port_open(void)
{
	// Declare variables and structures
		DCB dcbSerialParams = { 0 };
		COMMTIMEOUTS timeouts = { 0 };

		// Open the right serial port number, normally on this computer should be COM1
		fprintf(stderr, "Opening serial port...");

		serial_port = CreateFile(
			"\\\\.\\COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (serial_port == INVALID_HANDLE_VALUE)
		{
			fprintf(stderr, "Error\n");
			return 1;
		}
		else fprintf(stderr, "OK\n");


		// Set device parameters (9600 baud, 1 start bit,
		// 1 stop bit, no parity)
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (GetCommState(serial_port, &dcbSerialParams) == 0)
		{
			fprintf(stderr, "Error getting device state\n");
			CloseHandle(serial_port);
			return 1;
		}

		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (SetCommState(serial_port, &dcbSerialParams) == 0)
		{
			fprintf(stderr, "Error setting device parameters\n");
			CloseHandle(serial_port);
			return 1;
		}

		// Set COM port timeout settings
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (SetCommTimeouts(serial_port, &timeouts) == 0)
		{
			fprintf(stderr, "Error setting timeouts\n");
			CloseHandle(serial_port);
			return 1;
		}
		// Set COM port timeout settings
			timeouts.ReadIntervalTimeout = 50;
			timeouts.ReadTotalTimeoutConstant = 50;
			timeouts.ReadTotalTimeoutMultiplier = 10;
			timeouts.WriteTotalTimeoutConstant = 50;
			timeouts.WriteTotalTimeoutMultiplier = 10;
			if (SetCommTimeouts(serial_port, &timeouts) == 0)
				{
					fprintf(stderr, "Error setting timeouts\n");
					CloseHandle(serial_port);
					return 1;
				}
}

// Writes data to the port.
// Parameter:  write_buffer - the data to write to the port.
// *write_buffer - the buffer that contains the data to write.

int serial_port_write(char *write_buffer)
{
	DWORD bytes_written, total_bytes_written = 0;
	
	cout<<write_buffer<<endl;
	fprintf(stderr, "Sending bytes...");
		if (!WriteFile(serial_port, write_buffer, 3, &bytes_written, NULL))
		{
			fprintf(stderr, "Error\n");
			CloseHandle(serial_port);
			return 1;
		}
		fprintf(stderr, "%d bytes written\n", bytes_written);

}


class Command {
    public:
       virtual void action(int val) = 0;
    protected:
       virtual ~Command()=0;
};

Command::~Command(){};



class RingCommand : public Command {
    public:
       void action(int val) { cout << "Ring the bell" << endl;
       char bytes_to_send[3];
       	bytes_to_send[0] = 0xFE;
       	bytes_to_send[1] = 0x0B;
       	bytes_to_send[2] = 0x9D;
       	serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class Haul1Command : public Command {
    public:
       void action(int val) { cout << "Haul1" << endl;
       char bytes_to_send[3];
       	bytes_to_send[0] = 0xFE;
       	bytes_to_send[1] = 0x0B;
       	bytes_to_send[2] = 0x9C;
       	serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class Haul2Command : public Command {
    public:
       void action(int val) { cout << "Haul2" << endl;
       char bytes_to_send[3];
       	bytes_to_send[0] = 0xFE;
       	bytes_to_send[1] = 0x0B;
       	bytes_to_send[2] = 0x9F;
       	serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class LetoffsoundCommand : public Command {
    public:
       void action(int val) { cout << "let off sound" << endl;
       char bytes_to_send[3];
       	bytes_to_send[0] = 0xFE;
       	bytes_to_send[1] = 0x0B;
       	bytes_to_send[2] = 0x9E;
       	serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class moveCommand : public Command {
    public:
       void action(int val) { cout << "move the train at :"<<val<< endl;
       	   	   	char bytes_to_send[3];
             	bytes_to_send[0] = 0xFE;
             	bytes_to_send[1] = 0x0B;
             	bytes_to_send[2] = 0xE0+val%15;
             	cout << bytes_to_send[2] << " "<<val<<endl;
             	int res=serial_port_write(bytes_to_send);
             	cout<<"serial response"<<res<<endl;
       }
       friend class Command;
};

class decelerateCommand : public Command {
    public:
       void action(int val) { cout << "decelerate" << endl;
       char bytes_to_send[3];
                    	bytes_to_send[0] = 0xFE;
                    	bytes_to_send[1] = 0x0B;
            for(int i=1;i<val+1;i++){
                     		sleep(1);
                         	bytes_to_send[2] = 0xC5-1;
                             serial_port_write(bytes_to_send);
                          	cout << "decelerate 1" << endl;
             }
       }
       friend class Command;
};

class accelerateCommand : public Command {
    public:
       void action(int val) { cout << "accelerate" << endl;
       char bytes_to_send[3];
                    	bytes_to_send[0] = 0xFE;
                    	bytes_to_send[1] = 0x0B;
       for(int i=1;i<val;i++){
    		sleep(1);
        	bytes_to_send[2] = 0xC5+1;
        	cout << "accelerate 1" << endl;
            serial_port_write(bytes_to_send);
       }

       }
       friend class Command;
};

class initCommand : public Command {
    public:
       void action(int val) { cout << "init train" << endl;
       char bytes_to_send[3];
              	bytes_to_send[0] = 0xFE;
              	bytes_to_send[1] = 0x0B;
              	bytes_to_send[2] = 0x9C;
              	serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class directionCommand : public Command {
    public:
       void action(int val) { cout << "move" << endl;
       char bytes_to_send[3];
                    	bytes_to_send[0] = 0xFE;
                    	bytes_to_send[1] = 0x0B;
                    	bytes_to_send[2] = 0x80+(val>=0?0:3);
                    	cout << (val>=0?"forward":"reverse") << endl;
                    	serial_port_write(bytes_to_send);
       }
       friend class Command;
};



class stopCommand : public Command {
    public:
       void action(int val) { cout << "stop" << endl;
       char bytes_to_send[3];
                    	bytes_to_send[0] = 0xFE;
                    	bytes_to_send[1] = 0x0B;
                    	bytes_to_send[2] = 0xE0;
       serial_port_write(bytes_to_send);
       }
       friend class Command;
};

class CommandFactory {
    public:
        static Command* Create(string type){
        if ( type == "ring" ) return new RingCommand();
        if ( type == "move" ) return new moveCommand();
        if ( type == "haul1" ) return new Haul1Command();
        if ( type == "haul2" ) return new Haul2Command();
        if ( type == "letoff" ) return new LetoffsoundCommand();
        if ( type == "stop" ) return new stopCommand();
        if ( type == "accelarate" ) return new accelerateCommand();
        if ( type == "decelarate" ) return new decelerateCommand();
        if ( type == "direction" ) return new directionCommand();
        if ( type == "start" ) return new initCommand();

        return NULL;
    }
};




void parseCommand(int argv,char *args[]){
	string command=args[1];
	int val=0;
	if(argv>2)
		val=atoi(args[2]);
	Command* cmd = CommandFactory::Create(command);
	cmd->action(val);
}


int main( int argc, char *argv[] ) {



	serial_port_open();
	//
	parseCommand(argc,argv);


	serial_port_close();

}


