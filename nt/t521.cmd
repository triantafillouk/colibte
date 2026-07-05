# types ok from 908 TBC not ok with normal!
print("--- t521.cmd")
type  human1 = [name::"Name" surname::"Surname" age::0]
a=1
b=2
# show_vars()
c = human1[2]("name1","tr1",33; "name2","tr2",43)
c:
# show_vars()
# print(c)
c[1].surname:
# s1=c[1].surname
# s1:
c[1].surname="new surname"
show_vars()
c[1].surname:
show_vars()
s1=c[1].surname
c[0].surname:
# show_vars()