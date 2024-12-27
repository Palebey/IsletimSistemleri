/*
G231210355 Metehan Gülşer
B231210355 Emir Kır
G231210387 Yiğit Alp Çakır
B211210006 Muhammet Enes Nas
B221210003 Akif Emre Yaman
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define MAX_LINE 1024


// cd komutunun işlenmesi
void handle_cd(char* path) {
    if (chdir(path) != 0) {
        perror("cd error");
    }
}

// help komutunun işlenmesi
void handle_help() {
    printf("Commands:\n");
    printf("  cd <directory>  - Change directory\n");
    printf("  help            - Show this help message\n");
    printf("  quit            - Exit the shell\n");
    printf("  increment <num> - Increment a number\n");
    printf("  sleep <sec>     - Sleep for a number of seconds\n");
    printf("  ls              - List directory contents\n");
    printf("  cat <filename>  - Display contents of a file\n");
}

// increment komutunun işlenmesi
void handle_increment(int num) {
    printf("Incremented number: %d\n", num + 1);
}

// quit komutunun işlenmesi
 void handle_quit() {
    printf("Exiting shell...\n");
    exit (0);
}

// echo komutunun işlenmesi
void handle_echo(char *arg) {
    printf("%s\n", arg);  // Echo komutunun çıktısını yazdır
}

// sleep komutunun işlenmesi
void handle_sleep(int sec) {
    printf("Sleeping for %d seconds...\n", sec);
    sleep(sec);  // Belirtilen süre kadar bekle
}

// ls komutunun işlenmesi
void handle_ls() {
    // ls komutunu çalıştır
    system("ls");
}

// cat komutunun işlenmesi (dosyanın içeriğini yazdırır)

void handle_cat(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("cat error");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);  // Dosya içeriğini yazdır
    }

    fclose(file);
}


void handle_pipe_and_semicolon(char* input) {
    // Pipe işaretini kontrol et
    char* pipe_cmds[2];
    pipe_cmds[0] = strtok(input, "|");
    pipe_cmds[1] = strtok(NULL, "|");

    if (pipe_cmds[1] != NULL) {
        // Pipe komutları
        int pipefd[2];
        pid_t pid1, pid2;

        if (pipe(pipefd) == -1) {
            perror("Pipe failed");
            return;
        }

        pid1 = fork();
        if (pid1 == 0) {  // İlk çocuk işlem (echo komutu)
            close(pipefd[0]);  // Pipe'ın okuma ucunu kapat
            dup2(pipefd[1], STDOUT_FILENO);  // Yazma ucunu stdout'a yönlendir
            close(pipefd[1]);

            // echo komutunun işlenmesi
            char* token = strtok(pipe_cmds[0], " \t\r\n");
            if (token != NULL && strcmp(token, "echo") == 0) {
                token = strtok(NULL, " \t\r\n");
                if (token != NULL) {
                    printf("%s\n", token);  // echo çıktısını yazdır
                }
            }

            exit(0);
        }

        pid2 = fork();
        if (pid2 == 0) {  // İkinci çocuk işlem (increment komutu)
            close(pipefd[1]);  // Pipe'ın yazma ucunu kapat
            dup2(pipefd[0], STDIN_FILENO);  // Pipe'ı stdin'e yönlendir
            close(pipefd[0]);

            char buffer[256];
            int num;
            // Pipe'dan gelen değeri al
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                num = atoi(buffer);
                handle_increment(num);  // increment işlemi
            }

            exit(0);
        }

        // Ebeveyn sürecin işlerinin tamamlanmasını beklemek
        close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL);
        wait(NULL);
    } else {
        // Art arda çalıştırma ; komutu
        char* semicolon_cmds[10];
        int idx = 0;

        semicolon_cmds[idx] = strtok(input, ";");
        while (semicolon_cmds[idx] != NULL) {
            idx++;
            semicolon_cmds[idx] = strtok(NULL, ";");
        }

        for (int i = 0; i < idx; i++) {
            char *cmd_copy = strdup(semicolon_cmds[i]);  // Yedek olarak cmd'yi ayır
            execute_command(cmd_copy);
            free(cmd_copy);
        }
    }
}
void execute_command(char* input) {
    char* token;
    char* args[MAX_LINE / 2 + 1];  // Komutları saklamak için dizi
    int background = 0;  // Arka planda çalışacak mı?
    int redirection = 0; // Çıkış yönlendirme işareti var mı?
    char* output_file = NULL;  // Çıkış dosyası

    token = strtok(input, " \t\r\n");  // Komutu ilk parçaya ayır
    int i = 0;

    // Arka planda çalışacak komutların belirlenmesi
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;  // Eğer '&' var ise arka planda çalıştır
            break;  // '&' işaretinden sonrası okunmaz
        }
        if (strcmp(token, ">") == 0) {
            redirection = 1;  // Çıkış yönlendirme
            token = strtok(NULL, " \t\r\n");
            output_file = token;  // Dosya adını al
            break;
        }
        args[i++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    args[i] = NULL;  // Komut listemizi sonlandır

    if (args[0] == NULL) {
        return;  // Boş girdi, işlem yapma
    }

    pid_t pid = fork();
    if (pid == 0) {  // Çocuk process
        // Eğer çıkış yönlendirmesi varsa
        if (redirection) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                perror("Redirection failed");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);  // Standart çıktıyı dosyaya yönlendir
            close(fd);
        }

        // cd komutu
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] != NULL) {
                handle_cd(args[1]);
            } else {
                printf("cd: missing argument\n");
            }
        }
        // help komutu
        else if (strcmp(args[0], "help") == 0) {
            handle_help();
        }
        // quit komutu
        else if (strcmp(args[0], "quit") == 0) {
            handle_quit();
        }
        // increment komutu
        else if (strcmp(args[0], "increment") == 0) {
            if (args[1] != NULL && isdigit(args[1][0])) {
                handle_increment(atoi(args[1]));
            } else {
                printf("increment: invalid argument. Provide a number.\n");
            }
        }
        // echo komutu
        else if (strcmp(args[0], "echo") == 0) {
            if (args[1] != NULL) {
                handle_echo(args[1]);  // echo çıktısını ver
            } else {
                printf("echo: missing argument\n");
            }
        }
        // sleep komutu
        else if (strcmp(args[0], "sleep") == 0) {
            if (args[1] != NULL && isdigit(args[1][0])) {
                handle_sleep(atoi(args[1]));  // Süre kadar uyuma
            } else {
                printf("sleep: invalid argument. Provide a valid number of seconds.\n");
            }
        }
        // ls komutu
        else if (strcmp(args[0], "ls") == 0) {
            handle_ls();  // ls komutunu çalıştır
        }
        // cat komutunun işlenmesi
        else if (strcmp(args[0], "cat") == 0) {
            if (args[1] != NULL) {
                handle_cat(args[1]);  // cat komutunu çalıştır
            } else {
                printf("cat: missing argument. Provide a filename.\n");
            }
        }
        // Bilinmeyen komut
        else {
            printf("Unknown command: %s\n", args[0]);
        }
        exit(0);
    }
    else if (pid > 0) {  // Ebeveyn process
        if (!background) {  // Arka planda çalışmayacaksa
            int status;
            waitpid(pid, &status, 0);  // Çocuğun bitmesini bekle
        } else {
            // Arka planda çalışıyorsa, hemen komut istemine dön
            printf("[%d] running in background\n", pid);
            // İşlem bitiminde sonuçları yazdır
            int status;
            waitpid(pid, &status, WNOHANG);  // Arka planda devam etmesine izin ver
            if (WIFEXITED(status)) {
                printf("[%d] retval: %d\n", pid, WEXITSTATUS(status));
            }
        }
    } else {
        perror("fork failed");
    }
}
int main() {
    char input[MAX_LINE];
printf("##################\n##################\n###### SHELL #####\n##################\n##################\n\n\n");
    while (1) {
        
        printf("shell> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // EOF sinyali alındığında döngüden çık
        }

        // Satır sonu karakterini kaldır
        input[strcspn(input, "\n")] = 0;

        // 'exit' veya 'quit' komutu kontrolünü sağla
        if (strcmp(input, "quit") == 0) {
            handle_quit();
        } 

        // Pipe'ı veya art arda gelen komutları işle
    
        handle_pipe_and_semicolon(input);
    }

    return 0;
}