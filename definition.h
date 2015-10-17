int ARGC;
char **ARGV;

#define ALIASLEN 40
#define BUFFSIZE 1024
#define OPTLEN 16

struct PACKET {
        char option[OPTLEN]; // instruction
        char alias[ALIASLEN]; // client's alias
        char buff[BUFFSIZE];
        char connectTo[20];
        int list;
        char names[1024];
        int nameCounter;
};
