/*
G231210355 Metehan Gülşer
B231210355 Emir Kır
G231210387 Yiğit Alp Çakır
B211210006 Muhammet Enes Nas
B221210003 Akif Emre Yaman
*/

#define TRUE 1
#define FALSE !TRUE
#define LIMIT 256 // max number of tokens for a command
#define MAXLINE 1024 // max number of characters from user input


#define MAX_COMMANDS 3   // Maksimum komut sayısı
#define MAX_ARGS 10      // Maksimum argüman sayısı
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

static char* currentDirectory;
extern char** environ;

void sig_chld(int);
int os_launch(char **args, int background);
int os_launchbg(char **args);
void inputRD(char *args[],char* inputFile);
void outputRD(char *args[],char* inputFile);
int os_cd(char **args);
int os_help(char **args);
int os_quit(char **args);
int os_increment(char **args);
int commandHandler(char *args[]);
int Pipe_Handler(const char *number);
int executeCommand(char *args[]);
int executePipe(char *leftArgs[], char *rightArgs[]);