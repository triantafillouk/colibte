# types ok from 908 TBC not ok with normal!
cls
print("--- t52.cmd")
type  human1 = [name::"Name" surname::"Surname" age::0]
k=1

c = human1[2]("name1","tr1",33; "name2","tr2",43)

print(c)
s2="test surname"

c[1].surname=s2
k:   1.0
c[1].surname: "test surname"
show_vars()
