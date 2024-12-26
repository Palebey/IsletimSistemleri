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