#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

const char *BODY_CSS[] = {
	"background-color: #000",
	"color: #fff",
	NULL
};

int main() {
	Control header = (Control){ .Tag = HEAD_TAG, .CSS = (char **)BODY_CSS};

	Control text1 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no\" name=\"no\""};
	Control text2 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no1\" name=\"no1\""};
	Control text3 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no2\" name=\"no2\""};

	Control *design[] = { 
		&header, 
		&text1,
		&text2,
		&text3,
		NULL
	};

	String resp = ConstructDesign(design);

	printf("%s\n", resp.data);
	return 0;
}
