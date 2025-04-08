
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  const char *version = "1.0 BETA";
  printf("EstProG Tweak Android %s\n", version);

  int null_fd = open("/dev/null", O_WRONLY);
  dup2(null_fd, STDOUT_FILENO);
  dup2(null_fd, STDERR_FILENO);
  close(null_fd);

  if (argc > 1 && strcmp(argv[1], "--install") == 0) {
    char command[256];
    snprintf(command, sizeof(command),
             "su -lp 2000 -c \"cmd notification post -S bigtext -t 'EstProG "
             "Tweak Android' 'Tag' 'Version: %s - Module installed "
             "successfully.'\"",
             version);
    system(command);
    exit(0);
  }

  char result[1024];

  for (int i = 0; i < 2; i++) {
    FILE *fp = popen(
        "getprop | grep -E 'logd|thermal' | cut -d '[' -f2 | cut -d ']' -f1",
        "r");
    if (fp != NULL) {
      while (fgets(result, sizeof(result) - 1, fp) != NULL) {
        result[strcspn(result, "\r\n")] = 0;
        char properties[64];
        snprintf(properties, sizeof(properties), "getprop %s", result);
        FILE *f = popen(properties, "r");
        if (f != NULL) {
          char output[64];
          fgets(output, sizeof(output), f);
          pclose(f);
          if (strcmp(output, "running\n") == 0 ||
              strcmp(output, "restarting\n") == 0) {
            char service[64];
            snprintf(service, sizeof(service), "setprop ctl.stop %s",
                     result + 9);
            system(service);
          }
        }
      }
      pclose(fp);
    }
    sleep(5);
  }

  for (int i = 0; i < 2; i++) {
    FILE *fp = popen(
        "getprop | grep -E 'logd|thermal' | cut -d '[' -f2 | cut -d ']' -f1",
        "r");
    if (fp != NULL) {
      while (fgets(result, sizeof(result) - 1, fp) != NULL) {
        result[strcspn(result, "\r\n")] = 0;
        char properties[64];
        snprintf(properties, sizeof(properties), "getprop %s", result);
        FILE *f = popen(properties, "r");
        if (f != NULL) {
          char output[16];
          fgets(output, sizeof(output), f);
          pclose(f);
          if (strcmp(output, "running\n") == 0 ||
              strcmp(output, "restarting\n") == 0) {
            char service[64];
            snprintf(service, sizeof(service), "stop %s", result + 9);
            system(service);
            system(properties);
          }
        }
      }
      pclose(fp);
    }
    sleep(5);
  }

  sleep(10);
  FILE *fp;
  char pid[16];
  fp = popen("ps -e -o pid,comm | grep 'thermal' | awk '{print $1}'", "r");
  if (fp != NULL) {
    while (fgets(pid, sizeof(pid), fp) != NULL) {
      pid[strcspn(pid, "\r\n")] = 0;
      pid_t process_pid = atoi(pid);
      kill(process_pid, SIGSTOP);
    }
    pclose(fp);
  }

  for (int i = 0; i < 2; i++) {
    FILE *fp = popen(
        "getprop | grep -E 'logd|thermal' | cut -d '[' -f2 | cut -d ']' -f1",
        "r");
    if (fp != NULL) {
      while (fgets(result, sizeof(result) - 1, fp) != NULL) {
        result[strcspn(result, "\r\n")] = 0;
        char properties[64];
        snprintf(properties, sizeof(properties), "getprop %s", result);
        FILE *f = popen(properties, "r");
        if (f != NULL) {
          char output[16];
          fgets(output, sizeof(output), f);
          pclose(f);
          if (strcmp(output, "running\n") == 0 ||
              strcmp(output, "restarting\n") == 0) {
            snprintf(properties, sizeof(properties), "setprop %s paused",
                     result);
            system(properties);
          }
        }
      }
      pclose(fp);
    }
    sleep(5);
  }

  sleep(10);
  system("find /sys/devices/virtual/thermal -type f -exec chmod 000 {} +");

  return 0;
}