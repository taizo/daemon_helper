#ifndef HELPER_HELPER_H
#define HELPER_HELPER_H

#define VERSION			"0.0.1"

#define MAX_BUFF		128
#define DEF_INTERVAL	60
#define EXECUTION_DIR	"/tmp"

#ifndef LOCK_FILE
#define LOCK_FILE	"helper.lock"
#endif

#ifndef LOG_FILE
#define LOG_FILE	"helper.log"
#endif

#ifndef CONFIG_FILE
#define CONFIG_FILE     "\"/etc/helper.conf\""
#endif

void log_message(char *filename, char *message);
void daemonize(void);
void signal_handler(int sig);

int startServ(char *command);
unsigned int getProcessID(char *process);
int readConfig(char *filename);

#endif
