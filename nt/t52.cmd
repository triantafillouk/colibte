type  human1 = [name::"Name" surname::"Surname" age::0 bdate::0 salary::1 age2::1]
k=1

c = human1[2]("name1","tr1",33,34,35,36; "name2","tr2",43,44,45,46)
show_vars()
print(c)
#c[1][1]:
print("---2")
c[1].surname:
s1=c[1].surname
s1:
print("------- surname now s1 is ",s1)
print("surname now is ",c[1].surname)
s2="new surname"
print("set the new surname!")
c[1].surname=s2
print("this must be the new surname! ",c[1].surname)
c[1].surname:
s3=c[1].surname
print("this is copied from new surname ",s3)
print("end")
# print(a[22].name)
show_vars()