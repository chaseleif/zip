#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "include/zip.h"

#define MAX_PATH 256

enum { filepath=0, dirpath=1, unknpath=2 };
static inline const unsigned char pathtype(char *path) {
  struct stat path_stat;
  return stat(path, &path_stat)!=0?unknpath:
          S_ISREG(path_stat.st_mode)?filepath:
          S_ISDIR(path_stat.st_mode)?dirpath:unknpath;
}
static inline void do_compress(struct zip_t *zip, const char *file) {
  zip_entry_open(zip, file);
  zip_entry_fwrite(zip, file);
  zip_entry_close(zip);
  return;
}
static inline void walkdir(struct zip_t *zip, const char *path) {
  char fullpath[MAX_PATH];
  memset(fullpath, 0, sizeof(char)*MAX_PATH);
  DIR *dir = opendir(path);
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,"..")) continue;
    const int written = snprintf(fullpath,sizeof(char)*MAX_PATH,
                                  "%s/%s",path,entry->d_name);
    if (written < 0 || written > sizeof(char)*MAX_PATH) {
      fprintf(stderr,"Error using path for \"%s\" !\n",entry->d_name);
      continue;
    }
    const unsigned char filetype = pathtype(fullpath);
    switch(filetype) {
      case filepath:
        do_compress(zip, fullpath);
        break;
      case dirpath:
        walkdir(zip, fullpath);
        break;
      default: break;
    }
  }
  closedir(dir);
  return;
}
static inline void print_usage(char *prog) {
  printf("Minimal zip utility\n");
  printf("Usage:\n");
  printf("%s file.zip [options] [file list]\n", prog);
  printf("Zip file(s) or folder(s)\n");
  printf("(Always recurses into directories, doesn\'t follow links)\n");
}
int main(int argc,char **argv) {
  int infilecount = 0;
  char *outname = NULL;
  for (int i=1;i<argc;++i) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'h') {
        print_usage(argv[0]);
        return 1;
      }
      ++i;
    }
    else if (!outname)
      outname = argv[i];
    else
      ++infilecount;
  }
  if (infilecount < 1) {
    print_usage(argv[0]);
    fprintf(stderr,"No files specified to zip!\n");
    return 1;
  }
  if (!outname) {
    print_usage(argv[0]);
    fprintf(stderr,"No output file specified!\n");
    return 1;
  }
  char **inputfiles = malloc(sizeof(char*)*infilecount);
  for (int i=1,inputi=-2;i<argc;++i) {
    if (argv[i][0] == '-')
      ++i;
    else if (++inputi >= 0)
      inputfiles[inputi] = argv[i];
  }
  struct zip_t *zip = zip_open(outname, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  for (int i=0;i<infilecount;++i) {
    const unsigned char filetype = pathtype(inputfiles[i]);
    switch(filetype) {
      case filepath:
        do_compress(zip, inputfiles[i]);
        break;
      case dirpath:
        walkdir(zip, inputfiles[i]);
        break;
      default: break;
    }
  }
  zip_close(zip);
  free(inputfiles);
  return 0;
}
