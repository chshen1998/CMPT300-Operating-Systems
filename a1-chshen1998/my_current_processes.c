#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool check_number(const char* string) {
	int len = strlen(string);
	for (int i=0; i<len; ++i) {
		if (!isdigit(string[i])) {
			return false;
		}
	}
	return true;
}

int main()
{
	struct dirent *dir;
	char *string;
	char line[256], path[256];
	FILE *file, *cmdline;	
	DIR *proc = opendir("/proc");
	file = fopen("./output1.txt", "w");
	
	if (proc)
	{
		while ((dir = readdir(proc)) != NULL)
		{
			string = dir->d_name;
			if (check_number(string)) {
				if (atoi(string) == 1) {
					fprintf(file, "%s", string);
				} else {
					fprintf(file, "\n%s", string);	
				}
				strcpy(path, "/proc/");
				strcat(path, string);
				strcat(path, "/cmdline");
				cmdline = fopen(path, "r");
				fgets(line, sizeof(line), cmdline);
				printf("%s\n" ,line);
			}
		}
	}
	closedir(proc);
	return 0;
}

