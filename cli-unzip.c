#include <stdio.h>
#include <stdlib.h>
#include "include/zip.h"

int on_extract_entry(const char *filename, void *arg) {
  static int filei = 0;
  printf("Extracted: %s (%d of %d)\n", filename, ++filei, *(int*)arg);
  return 0;
}
static inline void print_usage(char *prog) {
  printf("Minimal unzip utility\n");
  printf("Usage:\n");
  printf("%s file.zip [-d exdir]\n", prog);
  printf("Extract zip file(s) to current directory\n");
  printf("Optionally extract files to exdir\n");
}
int main(int argc,char **argv) {
  int infilecount = 0;
  char *outdirectory = NULL;
  for (int i=1;i<argc;++i) {
    if (argv[i][0] == '-' && argv[i][1] == 'd') {
      if (argv[i][2] != '\0') {
        outdirectory = strdup(argv[i]+2);
      }
      else if (++i == argc) {
        print_usage(argv[0]);
        fprintf(stderr,"The -d flag is used with no directory specified!\n");
        return 1;
      }
      else {
        outdirectory = strdup(argv[i]);
      }
    }
    else ++infilecount;
  }
  if (infilecount < 1) {
    print_usage(argv[0]);
    fprintf(stderr,"No zip files specified to unzip!\n");
    return 1;
  }
  if (!outdirectory) {
    outdirectory = malloc(sizeof(char)*2);
    outdirectory[0]='.'; outdirectory[1]='\0';
  }
  char **inputfiles = malloc(sizeof(char*)*infilecount);
  for (int i=1,inputi=0;i<argc;++i) {
    if (argv[i][0] == '-' && argv[i][1] == 'd') {
      if (argv[i][2] == '\0') ++i;
    }
    else inputfiles[inputi++] = argv[i];
  }
  for (int i=0;i<infilecount;++i) {
    zip_extract(inputfiles[i],outdirectory,on_extract_entry,&infilecount);
  }
  free(outdirectory);
  free(inputfiles);
  return 0;
}
