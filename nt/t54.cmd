# type var in function TBD

type human = [name::"Name",age::20]

a=human("No name",100)

function pass1(r1,k)
{
	print("pass1: before changing age:",r1.age)
	r1.name="kostas"
	r1.age =30*k
	r1.age:
}
 
print("Start test")


a:
p=2
pass1(a,p)
a:

print("a.age=",a.age)
print("a.name=",a.name)
print("p=",p)