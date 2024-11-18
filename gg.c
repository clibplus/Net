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
	Control header = (Control){ .Tag = HEAD_TAG };
	Control title = (Control){ .Tag = TITLE_TAG, .Text = "cLib+ Web Server | cWD Framework\0" };

	Control *header_controls[] = {&header, &title, NULL};
	String header_html = ConstructDesign(header_controls);

	Control body = (Control){ .Tag = BODY_TAG, .CSS = (char **)BODY_CSS};

	Control text1 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no\" name=\"no\"\0"};
	Control text2 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no1\" name=\"no1\"\0"};
	Control text3 = (Control){ .Tag = P_TAG, .Text = "Test\0", .Data = "id=\"no2\" name=\"no2\"\0"};

	Control *body_design[] = { 
		&body, 
		&text1,
		&text2,
		&text3,
		NULL
	};

	String resp = ConstructDesign(body_design);

	printf("<html>%s%s</html>\n", header_html.data, resp.data);
	return 0;
}
