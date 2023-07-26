assign_sub ("show_out","M-,")

function show_out()
{
 if(!in_view("[out]")) {
	vsplit_window()
	next_window()
	select_file("[out]")
	prev_window()
 }
}

cls

