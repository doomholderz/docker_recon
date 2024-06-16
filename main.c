#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/capability.h>
#include <sys/capability.h>
#include <sys/types.h>
#include <unistd.h>


/* Capabilities in /proc/PID/status are hexadecimal bitmasks where each bit is a capability
 * We have three sections of capabilities
 * 	CapInh (Inherited Capabilities)
 * 	CapPrm (Permitted Capabilities)
 * 	CapEff (Effective Capabilities)
 *
 * Once we have got these strings we convert these into cap_value_t structures using cap_from_text
 * tranlsating textual representation in /status into bitmask structure (where bits = capabilities)
 *
 * With this cap_value_t structure we can iterate through each bit to check if a capability is set
 *	To do this we use cap_to_name, which maps the bit position to capability name
 */

void decode_capability_bitmask(const char* cap_str, const char* cap_type) {
	int len = strlen(cap_str);
	char binary_str[(len+1) * 4]; // Each hex character is 4 bits, plus one for null
	int bit_index = 0;

	// Iterate through each character of cap_str
	for (int i = 0; i < len; ++i) {
		unsigned int value;
		sscanf(cap_str + i, "%1x", &value);
		sprintf(binary_str + bit_index, "%04b", value);
		bit_index += 4;
	}
	printf("%s\n", binary_str);
}

void get_capabilities() {
	FILE *fp = fopen("/proc/1/status", "r");
	if (!fp) {
		perror("Failed to open");
		return;
	}

	char line[1000];
	char cap_inh[50] = {0};
	char cap_prm[50] = {0};
	char cap_eff[50] = {0};

	while (fgets(line, sizeof(line), fp)) {
		if (strncmp(line, "CapInh:", 7) == 0) {
			sscanf(line + 7, "%s", cap_inh);
		}
		if (strncmp(line, "CapPrm:", 7) == 0) {
			sscanf(line + 7, "%s", cap_prm);
		}
		if (strncmp(line, "CapEff:", 7) == 0) {
			sscanf(line + 7, "%s", cap_eff);
		}
	}

	fclose(fp);

	if (strlen(cap_inh) > 0) {
		printf("Inherited capabilities:\n");
		decode_capability_bitmask(cap_inh, "CapInh");
	}

	if (strlen(cap_prm) > 0) {
		printf("Permitted capabilities:\n");
		decode_capability_bitmask(cap_prm, "CapPrm");
	}

	if (strlen(cap_eff) > 0) {
		printf("Effective capabilities:\n");
		decode_capability_bitmask(cap_eff, "CapEff");
	}


}

int main() {
	get_capabilities();
	return 0;
}
