#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "stressball.h"

int main (int ac, char **av)
{
  int x, fd;
  char *s;

  fd = open(av[1], O_RDWR);

  if (fd < 0) {
    perror ("open");
    exit(1);
  }

  while (1) {
    ioctl(fd, STRESSBALL_GET_STATUS, &x);
    s = (char *)x;
    printf("Stressball Status: %d %d %d", s[0], s[1], s[2]);
    usleep (200000);
  }

  close(fd);
  return 0;
}
