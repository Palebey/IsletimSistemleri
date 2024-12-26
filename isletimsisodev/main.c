/*
G231210355 Metehan Gülşer
B231210355 Emir Kır
G231210387 Yiğit Alp Çakır
B211210006 Muhammet Enes Nas
B221210003 Akif Emre Yaman
*/

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