#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE *output, *os_release, *version;
    char line[256];

    os_release = fopen("/etc/os-release", "r");
    version = fopen("/proc/version", "r");
	output = fopen("./output3.txt", "a");

    if (os_release) {
        while(fgets(line, sizeof(line), os_release)) {
            fprintf(output, "%s", line);
		}
    }

    if (version) {
        while(fgets(line, sizeof(line), version)) {
            fprintf(output, "%s", line);
		}       
    }

    fclose(os_release);
    fclose(version);
    fclose(output);

    return 0;
}