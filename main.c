#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define ARRAY_LENGHT(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef enum {
    SAMP,
    ARIZONA
} GameType;

typedef struct {
    const char *ip;
    const char *port;
    const char *servername;
    const char *nickname;
} Server;

#include "config.h"

char *priorityNickname;

void launch_game(const Server *server, GameType game)
{   
    int cdresult;
    switch (game) {
        case SAMP:
            cdresult = chdir(path_to_gtasa);
            break;
        case ARIZONA:
            cdresult = chdir(path_to_arizona);
            break;
    }
    
    if (cdresult == -1)
    {
        printf("Error: %s\n", strerror(errno));
        exit(-1);
    }
    
    if (fork() == 0)
    {
        int logfd = open("/tmp/cli-samp-client.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (logfd == -1) _exit(126);

        dup2(logfd, STDOUT_FILENO);
        dup2(logfd, STDERR_FILENO);

        if (logfd > STDERR_FILENO) close(logfd);
        setsid();

        if (EXPORT_RU_LOCALE == 1) 
        {
            setenv("LC_ALL", "ru_RU.UTF-8", 1);
        }

        char *nickname;        
        if (priorityNickname) {
            nickname = priorityNickname;
        } else {
            nickname = (char *)server->nickname;
        }
        
        switch (game) {
            case SAMP: {
                pid_t regpid = fork();
                if (regpid == 0)
                {
                    char *reg_argv[] = {"wine", "reg", "add", "HKCU\\Software\\SAMP", "/v", "gta_sa_exe", "/t", "REG_SZ", "/d", path_to_gtasa_exe, "/f", NULL};
                    execvp("wine", reg_argv);
                    _exit(1);
                }
                int status;
                waitpid(regpid, &status, 0);

                char address[64];
                char nicknameArg[256];
                snprintf(address, sizeof(address), "%s:%s", (char *)server->ip, (char *)server->port);
                snprintf(nicknameArg, sizeof(nicknameArg), "-n%s", nickname);

                char *args[] = { "wine", "samp.exe", address, nickname, NULL };        
                execvp("wine", args);
                break;
            }
            case ARIZONA: {
                char *args[] = { "wine", "gta_sa.exe", "-arizona", "-c", "-h", (char *)server->ip, "-p", (char *)server->port, "-mem", DEFAULT_MEM, "-n", nickname, NULL };
                execvp("wine", args);
                break;
            }
        }
        fprintf(stderr, "exec wine failed: %s\n", strerror(errno));
        _exit(127);
    }
}

int main(int argc, char *argv[])
{
    GameType game = SAMP;
    if (argc > 1) {
        int opt;
        static struct option long_options[] = {
            { "list", no_argument, 0, 'l' },
            { "help", no_argument, 0, 'h' },
            { "arizona", no_argument, 0, 'a' },
            { "nick", required_argument, 0, 'n' },
            { 0, 0, 0, 0 }
        };

        while ( (opt = getopt_long(argc, argv, "lhan:", long_options, NULL)) != -1 )
        {
            switch (opt) {
                case 'h':
                    printf("Usage: <%s>\n\
    Without args        Launch samp client\n\
    -h, --help          Show this screen\n\
    -l, --list          Show saved servers\n\
    -a, --arizona       Launch arizona client\n\
    -n, --nick          Set priority nickname. This nickname will be used instead that in config.h\n", argv[0]);
                    exit(0);
                case 'a':
                    game = ARIZONA;
                    break;
                case 'n':
                    priorityNickname = optarg;
                    printf("Usage nickname: %s\n", priorityNickname);
                    break;
                case 'l':
                    size_t total_servers = ARRAY_LENGHT(servers);
                    printf("Saved servers [Total: %zu]\n", total_servers);                    
                    for (size_t i = 0; i < total_servers; i++)
                    {
                        printf("%s[%s:%s] - Nickname: %s\n", servers[i].servername, servers[i].ip, servers[i].port, servers[i].nickname);
                    }
                    exit(0);
                case '?':
                    exit(1);
                default:
                    break;
            }
        }
    }

    printf("Select server:\n");
    for ( size_t i = 0; i < ARRAY_LENGHT(servers); i++)
    {
        printf("%zu. | %s [%s:%s] - Nickname: %s\n", i + 1, servers[i].servername, servers[i].ip, servers[i].port, servers[i].nickname);
    }
    int choice;

    printf("Enter your choice (number): ");
    if (scanf("%d", &choice) == 1) {
        if (choice < 1 || (size_t)choice > ARRAY_LENGHT(servers)) {
            printf("You need input correctly server number!\n");
        } else {
            launch_game(&servers[choice - 1], game);
        }
    } else {
        printf("Input error. You should input number\n");
    }

    return 0;
}
