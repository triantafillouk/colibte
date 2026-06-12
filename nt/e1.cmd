# different final result, but it does not matter TBC
print("--- e1.cmd")
function show_out()
{
 if(!in_view("[out]")) {
	vsplit_window()
	next_window()
	select_file("[out]")
	prev_window()
 }
}

assign_sub ("show_out","M-,")

cls

