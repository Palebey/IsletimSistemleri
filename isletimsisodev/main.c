/*
G231210355 Metehan Gülşer
B231210355 Emir Kır
G231210387 Yiğit Alp Çakır
B211210006 Muhammet Enes Nas
B221210003 Akif Emre Yaman
*/

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <signal.h>

#include <sys/wait.h>

#include <fcntl.h>

#include <termios.h>
 
#include "main.h"




char *builtin_str[]={

    "cd",

    "help",

    "quit",

    "increment"

};



int (*builtin_func[])(char**) = {

    &os_cd,

    &os_help,

    &os_quit,

    &os_increment

};





int os_builtins()

{

    return sizeof(builtin_str)/sizeof(char *);

}



int Pipe_Handler(const char *number) {
    char *endptr;

    // String'den tam sayıya dönüşüm
    long num = strtol(number, &endptr, 10);

    // Geçersiz karakter kontrolü
    if (*endptr != '\0') {
        fprintf(stderr, "OsShell: increment: geçersiz sayı: %s\n", number);
        return 1;
    }

    // Sayıyı artır
    num++;

    // Artırılmış sayıyı yazdır
    printf("%ld\n", num);

    return 0; // Başarılı tamamlandı
}



int os_increment(char **args) {

    if (args[1] == NULL) {

        fprintf(stderr, "OsShell: increment: eksik argüman\n");

        return 1;

    }



    char *endptr;

    long num = strtol(args[1], &endptr, 10); // Sayıya dönüştür



    if (*endptr != '\0') {

        fprintf(stderr, "OsShell: increment: geçersiz sayı: %s\n", args[1]);

        return 1;

    }



    num++; // Sayıyı artır

    printf("%ld\n", num);

    return 1;

}



int os_cd(char **args)

{

    if (args[1]==NULL)

    {

        fprintf(stderr,"OsShell: expected argument to \"cd\"\n");

    }

    else

    {

        if (chdir(args[1])!=0)

        {

            perror("OsShell");

        }

    }

    return 1;

}



int os_help(char **args)

{

    int i;

    printf("Isletım Sistemleri Odevi\n");

    for (i=0;i<os_builtins();i++)

    {

        printf("  %s\n",builtin_str[i]);

    }

    printf("Use the man command for information on other programs.\n");

    return 1;

}



int os_quit(char **args)

{

    int status;

    while (!waitpid(-1,&status,WNOHANG)){}

    exit(0);

}



#define AUXMAX 256

#define AUXMAX 256
int commandHandler(char * args[])
{
	int i=0;
	int j=0;
	int fileDescriptor;
	int standartOut;
	int aux;
	char *args_aux[AUXMAX];
	int k;
	int background = 0;
	int status;
	
	while(args[j] != NULL)
	{
		if ((strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0))
		{
			break;
		}
		args_aux[j] = args[j];
		j++;
	}
	args_aux[j]=NULL;

	int m;
	if (args[0] == NULL)
	{
		return 1;
	}
	for (m = 0 ; m < os_builtins() ; m++)
	{
		if (strcmp(args[0],builtin_str[m])==0)
		{
			(*builtin_func[m])(args);
			return 1;
		}
	}
	while (args[i] != NULL && background == 0)
	{
		if (strcmp(args[i],"&") == 0)
		{
			background = 1;
		}
		else if (strcmp(args[i],"<") == 0)
		{
			if (args[i+1] == NULL )
			{
				printf ("Yeterli Arguman Yok\n");
			}
			inputRD(args_aux,args[i+1]);
			return 1;
		}
		else if (strcmp(args[i],">") == 0)
		{
			if (args[i+1] == NULL )
			{
				printf ("Yeterli Arguman Yok\n");
			}
			outputRD(args_aux,args[i+1]);
			return 1;
		}
		i++;
	}
	args_aux[i]==NULL;
	os_launch(args_aux,background);

	return 1;
}

int executePipe(char *leftArgs[], char *rightArgs[]) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Sol komut
    pid1 = fork();
    if (pid1 == 0) {
        // Pipe'in yazma ucuna yönlendir
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(leftArgs[0], leftArgs) == -1) {
            perror("Execution of left command failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid1 < 0) {
        perror("Fork failed for left command");
        return 1;
    }

    // Sağ komut
    pid2 = fork();
    if (pid2 == 0) {
        // Pipe'in okuma ucuna yönlendir
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Yerleşik komut kontrolü
        for (int i = 0; i < os_builtins(); i++) {
            if (strcmp(rightArgs[0], builtin_str[i]) == 0) {
                Pipe_Handler(leftArgs[1]); // Yerleşik komutu çalıştır
                exit(0); // Çocuk işlemi sonlandır
            }
        }

        // Eğer yerleşik değilse, dış komut olarak çalıştır
        if (execvp(rightArgs[0], rightArgs) == -1) {
            perror("Execution of right command failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid2 < 0) {
        perror("Fork failed for right command");
        return 1;
    }

    // Ana işlem
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 1;
}


int executeCommand(char *args[]) {
    int m;

    if (args[0] == NULL) {
        return 1;
    }

    // Yerleşik komutlar
    for (m = 0; m < os_builtins(); m++) {
        if (strcmp(args[0], builtin_str[m]) == 0) {
            return (*builtin_func[m])(args);
        }
    }

    // Dış komutlar
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("Fork failed");
    } else {
        waitpid(pid, NULL, 0);
    }

    return 1;
}




void inputRD(char *args[],char* inputFile)
{
	pid_t pid;
	if (!(access (inputFile,F_OK) != -1))
	{	
		printf("OsShell: %s adinda bir dosya bulunamadi\n",inputFile);
		return;
	}
	int err=-1;
	int fileDescriptor;
	if((pid=fork()) == -1)
	{
		printf("Child olusturulamadi\n");
		return;
	}
	if (pid==0)
	{
		fileDescriptor=open(inputFile, O_RDONLY, 0600);
		dup2(fileDescriptor,STDIN_FILENO);
		close(fileDescriptor);

		if (execvp(args[0],args)==err)	
		{
			printf("err");
			kill(getpid(),SIGTERM);
		} 
	}
	waitpid(pid,NULL,0);
}

void outputRD(char *args[],char* outputFile)
{
	pid_t pid;
	int err=-1;
	int fileDescriptor;
	if((pid=fork()) == -1)
	{
		printf("Child olusturulamadi\n");
		return;
	}
	if (pid==0)
	{
		fileDescriptor=open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
		dup2(fileDescriptor,STDOUT_FILENO);
		close(fileDescriptor);

		if (execvp(args[0],args)==err)	
		{
			printf("err");
			kill(getpid(),SIGTERM);
		} 
	}
	waitpid(pid,NULL,0);
}





int os_launchbg(char **args)

{

    pid_t pid;

    int status;



    struct sigaction act;

    act.sa_handler = sig_chld;

    sigemptyset(&act.sa_mask);

    act.sa_flags = SA_NOCLDSTOP;

    if (sigaction(SIGCHLD,&act,NULL)<0)

    {

        fprintf(stderr,"sigaction failed\n");

        return 1;

    }



    pid=fork();

    if (pid == 0)

    {

        if (execvp(args[0],args) == -1)

        {

            printf("Command not found");

            kill(getpid(),SIGTERM);

        }

        //exit(EXIT_FAILURE);

    }

    else if (pid < 0)

    {

        perror("OsShell");

    }

    else

    {

        printf("Proses PID:%d Degeriyle Olusturuldu\n",pid);

    }

    return 1; 

}



int os_launch(char **args,int background)

{

    if (background==0)

    {

        pid_t pid;

        int status;

        pid=fork();

        if (pid == 0)

        {

            if (execvp(args[0],args) == -1)

            {

                printf("Command not found");

                kill(getpid(),SIGTERM);

            }

        }

        else if (pid < 0)

        {

            perror("OsShell");

        }

        else

        {

            waitpid(pid,NULL,0);

        }

    }

    else

    {

        os_launchbg(args);

    }

    return 1; 

}



void sig_chld(int signo) {

    int status, child_val, chid;

    chid = waitpid(-1, &status, WNOHANG);  // Çocuk işlemleri için bekleme

    

    if (chid > 0) {  // Beklemeyen çocuk süreçlerin durumunu kontrol et

        if (WIFEXITED(status)) {

            child_val = WEXITSTATUS(status);  // Çocuk işlemin çıkış durumu

            printf("[%d] finished with status: %d\n", chid, child_val);

        }

    }

}





void OSScreen(){

        printf("\n\t############################################\n\n");

        printf("\t                   OS Shell  \n");

        

        

       printf("\n\t############################################\n");

        printf("\n\n");

}



void PromptBas()

{



    printf("OsShell > ");

}



// main fonksiyonu

int main(int argc, char **argv, char **envp) {

    OSScreen();

    char line[MAXLINE];

    char *commands[LIMIT];

    int status = 1;



    while (status) {

        PromptBas();

        memset(line, '\0', MAXLINE);

        fgets(line, MAXLINE, stdin);

        

        // Komutları ';' ile ayır

        int numCommands = 0;

        commands[numCommands] = strtok(line, ";\n");

        while (commands[numCommands] != NULL) {

            numCommands++;

            commands[numCommands] = strtok(NULL, ";\n");

        }



        // Her komutu sırayla çalıştır

        for (int i = 0; i < numCommands; i++) {

            char *tokens[LIMIT];

            int numTokens = 0;

            tokens[numTokens] = strtok(commands[i], " \t\n");

            while (tokens[numTokens] != NULL) {

                numTokens++;

                tokens[numTokens] = strtok(NULL, " \t\n");

            }



            if (tokens[0] != NULL) {

                status = commandHandler(tokens);

            }

        }

    }

    return 0;

}