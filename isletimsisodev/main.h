/*
G231210355 Metehan Gülşer
B231210355 Emir Kır
G231210387 Yiğit Alp Çakır
B211210006 Muhammet Enes Nas
B221210003 Akif Emre Yaman
*/

#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define MAX_LINE 1024

// Fonksiyon prototipleri
void handle_cd(char* path);
void handle_help();
void handle_increment(int num);
void handle_quit();
void handle_echo(char *arg);
void handle_sleep(int sec);
void handle_ls();
void handle_cat(char *filename);
void execute_command(char* input);
void handle_pipe_and_semicolon(char* input);

#endif