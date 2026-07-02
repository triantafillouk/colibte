# types ok from 908 TBC not ok with normal!
print("--- t521.cmd")
type  human1 = [name::"Name" surname::"Surname" age::0 bdate::0 salary::1 age2::1]

c = human1[2]("name1","tr1",33,34,35,36; "name2","tr2",43,44,45,46)
# show_vars()
# print(c)
c[1].surname:
# s1=c[1].surname
# s1:
c[1].surname="new surname"
c[1].surname:
# show_vars()