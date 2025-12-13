#ifndef UI_CONSTANT_H
#define UI_CONSTANT_H

static constexpr size_t header_element_count	= 7;
static constexpr size_t ui_pad_lines		= 8000;
static constexpr size_t ui_pad_columns		= 134;
static constexpr size_t ui_footer_lines		= 3;
static constexpr size_t ui_header_lines		= 3;
static constexpr size_t ui_help_lines		= 26;
static constexpr size_t ui_max_width		= 403;

static const char separator[ui_max_width] = "┃ %-10d ┃ %-24.23s ┃ %-32.31s ┃ %-10s ┃ %-6.1f %s ┃ %-6.1f%% ┃  %-18s ┃";

static constexpr char empty_separator[ui_max_width] = "┃ %-10s ┃ %-24s ┃ %-32s ┃ %-10s ┃ %-10s ┃ %-7s ┃  %-18s ┃";

static constexpr char proc_array_function_command[ui_footer_lines][ui_max_width] = {
	"┣━━━━━━━━━━━┳┻━━━━━━━━━━━━━┳━━━━━━━━━━━━┻━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━┻━━┳━━━━━━━━━┻━━━━━━┳━━━━━┻━━━━━┳━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] help ┃ [F2] page <- ┃ [F3] page -> ┃ [F4] search ┃ [F5] pause/continue ┃ [F6] terminate ┃ [F7] kill ┃ [F8] reload ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

static constexpr char proc_array_search_bar[ui_footer_lines][ui_max_width] = {
	"┣━━━━━━━━━━━━┻┳━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] cancel ┃ Search:                                                                                                  ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

static constexpr char proc_array_help_footer[ui_footer_lines][ui_max_width] = {
	"┣━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] cancel ┃                                                                                                          ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

static constexpr char proc_array_tab_header[ui_header_lines][ui_max_width] = {
        "┏━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓",
        "┃ PID      %s ┃ User                   %s ┃ Name                           %s ┃ State    %s ┃ RAM      %s ┃ CPU   %s ┃ Time(dd:hh:mm:ss) %s ┃",
        "┣━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━┫",
};

static constexpr char proc_array_help_header[ui_header_lines][ui_max_width] = {
        "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓",
	"┃                                                                                                                                    ┃",
	"┃                                                                                                                                    ┃",
};

static constexpr char proc_array_help[ui_help_lines][ui_max_width] = {
	"┃ ---------------------- G L O B A L ----------------------                                                                          ┃",
	"┃ [→]     scroll to right                                                                                                            ┃",
	"┃ [←]     scroll to left                                                                                                             ┃",
	"┃ [↑]     scroll to top                                                                                                              ┃",
	"┃ [↓]     scroll to bottom                                                                                                           ┃",
	"┃                                                                                                                                    ┃",
	"┃ [tab]   select sorting field                                                                                                       ┃",
	"┃ [enter] select ascendant/descendent sort                                                                                           ┃",
	"┃                                                                                                                                    ┃",
	"┃ [F9]    exit from the program                                                                                                      ┃",
	"┃                                                                                                                                    ┃",
	"┃ ----------------- N O R M A L ~ M O D E -----------------                                                                          ┃",
	"┃ [F1]    show the help page                                                                                                         ┃",
	"┃ [F2]    switch to the previous machine processus page                                                                              ┃",
	"┃ [F3]    switch to the next machine processus page                                                                                  ┃",
	"┃ [F4]    switch to the search mode                                                                                                  ┃",
	"┃ [F5]    pause/continue the selected processus                                                                                      ┃",
	"┃ [F6]    terminate the selected processus                                                                                           ┃",
	"┃ [F7]    kill the selected processus                                                                                                ┃",
	"┃ [F8]    reload the selected processus                                                                                              ┃",
	"┃                                                                                                                                    ┃",
	"┃ ----------------- S E A R C H ~ M O D E -----------------                                                                          ┃",
	"┃ [F1]    exit from search mode (go back to normal mode)                                                                             ┃",
	"┃ [F2]    go to the previous occurent that match                                                                                     ┃",
	"┃ [F3]    go to the next occurent that match                                                                                         ┃",
	"┃                                                                                                                                    ┃",
};

#endif /* UI_CONSTANT_H */
