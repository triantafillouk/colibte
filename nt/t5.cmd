# types TBD
type  human1 = [name::"Name" surname::"Surname" age::0 bdate::0 salary::1 age2::1]
#print(human2)
#list1 = list ()
#keys = keys ()
#andrew = human "Ανδρεας","Πέτρου",33,2022,900)
a = human1("kostas"+" maria","tr",33,45,22,23)
#a = human1: 
# print(a)
a: array 1:[human1] type [AMIXED] , slot 0 type=14 rows 1,cols 6
# a = human1 ("Kostas","Tr",55,20,100)
# a[0]: 
#max=1
#c = human1[2]("name1","tr1",33,34,35,36; "name2","tr2",43,44,45,46)
#c:
# fori(i=0;max;1){
	print(i,":===================")
	a.age:  33.0
	a.name: "kostas maria"
	a.age=77:  77.0
	a.age:  79.0
	a.age+=2:  77.0
	a.age:  79.0
	a.age-=2:  49.0
	a.age:  77.0
	k=a.age**2: 5929.0
	l=a.age%10:   7.0

	if(k==6241 & l==9) print("OK!")
#	a1=a.age: 
#	s1=a.salary: 
#}
# print(human1)
k=20
#b = human1[k*(k+2)]
c = human1[2]("name1","tr1",33,34,35,36; "name2","tr2",43,44,45,46)
c:
print("---1")
c[1][1]: "tr2"
print("---2")
c[1].name: "name1"
print("---3")
c[1].surname="new"+" surname"
print("---4")
c[1].surname:
print("end")
# print(a[22].name)
# show_vars()