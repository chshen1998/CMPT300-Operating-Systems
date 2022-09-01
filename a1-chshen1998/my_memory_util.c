#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *file, *meminfo, *output;
	char line[256];
	char *pch, *ptr;
	double total, free, buffers, cached, slab, utilization;

	meminfo = fopen("/proc/meminfo", "r");
	file = fopen("./meminfo.txt", "w");
	output = fopen("./output2.txt", "w");

	if (meminfo) {
		while(fgets(line, 256, meminfo) != NULL) {
			fprintf(file, "%s", line);
			pch = strtok(line, " ");
			if (strcmp(pch, "MemTotal:") == 0) {
				pch = strtok(NULL, " ");
				total = strtod(pch, &ptr);
			} else if (strcmp(pch, "MemFree:") == 0) {
				pch = strtok(NULL, " ");
				free = strtod(pch, &ptr);
			} else if (strcmp(pch, "Buffers:") == 0) {
				pch = strtok(NULL, " ");
				buffers = strtod(pch, &ptr);
			} else if (strcmp(pch, "Cached:") == 0) {
				pch = strtok(NULL, " ");
				cached = strtod(pch, &ptr);
			} else if (strcmp(pch, "Slab:") == 0) {
				pch = strtok(NULL, " ");
				slab = strtod(pch, &ptr);
			}
		}
	} else {
		printf("Meminfo not found");
	}

	utilization = ((total-free-buffers-cached-slab)/total)*100;
	fprintf(output, "%f", utilization);

	fclose(meminfo);
	fclose(file);
	fclose(output);

	return 0;
}
