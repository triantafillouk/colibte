# type speed test

type human = [name::"Name",age::20]

a=human("No name",100)

function pass1(r1,k)
{
	# print("pass1: before changing age:",r1.age)
	r1.name="kostas"
	r1.age =30*k
	# r1.age:
}

 
show_time("Start type speed test loop ",0)

result=0
loops=50000001

fori(i=0;loops;1) {
	pass1(a,i)
	result+=a.age
}
print("At the end of first loop:")
a.age:
result:
result=0
j=0	
show_time("mid1 test",1)
fori(i=0;loops;1) {
	result+=a.age
	j++
}
result:
show_time("mid2 test",1)
a.age*(loops-1):
j:
show_time("Test ended",2)

