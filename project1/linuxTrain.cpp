
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>



using namespace std;
int serial_port_open(void);
int serial_port_write(char *write_buffer);
int serial_port;
void serial_port_close(int);
struct termios options_original;
static const char *PORT_NAME = "/dev/ttyS3";


void serial_port_close(int fd){
        close(fd);
}




int serial_port_open(void)
{
  struct termios options;

  serial_port = open(PORT_NAME, O_RDWR);

  if (serial_port != -1)
  {
          printf("Serial Port open\n");
          int res1=tcgetattr(serial_port,&options_original);
          int res2=tcgetattr(serial_port, &options);
          int res3=cfsetispeed(&options, B9600);
          int res4=cfsetospeed(&options, B9600);
          options.c_cflag &= ~PARENB;
          options.c_cflag &= ~CSTOPB;
          //options.c_cflag &= ~CSIZE;
          options.c_cflag |= CS8;
          int res5=tcsetattr(serial_port, TCSADRAIN, &options);
          if(res1<0 || res2<0 || res3<<0 || res4<0 || res5<<0 )
                  cout<<"serial open failed"<<endl;
          cout<<"open serial port:"<<serial_port<<endl;
  }
  else
          printf("Unable to open %s",PORT_NAME);
  return (serial_port);
}

// Writes data to the port.
// Parameter:  write_buffer - the data to write to the port.
// *write_buffer - the buffer that contains the data to write.

int serial_port_write(char *write_buffer)
{
        int bytes_written;
        size_t len = 0;
        len = strlen(write_buffer);
        bytes_written = write(serial_port, write_buffer,len);
        if (bytes_written < len)
                printf("Write failed \n");
        return bytes_written;

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
        cout<<"command is "<<command<<endl;
        cmd->action(val);
}


int main( int argc, char *argv[] ) {



        serial_port_open();
        //
        parseCommand(argc,argv);

        serial_port_close(serial_port);

}

