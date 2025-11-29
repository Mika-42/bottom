#include "ui.h"

int main()
{
	ui_init();

	for(;;)
	{
		clear();
		ui_show_fn_cmd();
	}

	return 0;
}
