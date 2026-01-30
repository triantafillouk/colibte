# type var in function

type human = [name::"Name",age::20]

a=human

function pass1(r1,k)
{
	print("age:",r1.age)
	r1.age =30*k
	r1.name="kt"
}
 
p=2
pass1(a,p)

print("a.age=",a.age)
print("a.name=",a.name)
print("p=",p)