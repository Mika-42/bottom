define show_proc
	set $i = 0
	while $i < proc_list->size
		printf "%d %s\n", $i, proc_list->data[$i].user
		set $i = $i + 1
	end
end
