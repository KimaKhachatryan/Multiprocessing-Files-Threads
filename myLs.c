//Write a program that displays the same output on the screen as the result of the ls -l filename.txt command.

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_permissions(mode_t mode) {
    printf((S_ISREG(mode)) ? "-" : "?");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Correct usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct stat fileInfo;
    if (stat(argv[1], &fileInfo) < 0) {
        perror("stat");
        return 1;
    }

    //permissions
    print_file_permissions(fileInfo.st_mode);
    
    //hard links
    printf(" %ld", (long)fileInfo.st_nlink);

    //owner name
    struct passwd *pw = getpwuid(fileInfo.st_uid);
    printf(" %s", pw ? pw->pw_name : "");

    //group name
    struct group *gr = getgrgid(fileInfo.st_gid);
    printf(" %s", gr ? gr->gr_name : "");

    //file size
    printf(" %ld", (long)fileInfo.st_size);

    //time
    char timebuf[80];
    struct tm *tm = localtime(&fileInfo.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
    printf(" %s", timebuf);

    //file name
    printf(" %s\n", argv[1]);

    return 0;
}
