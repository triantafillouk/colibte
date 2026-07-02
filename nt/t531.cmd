# types ok from 908, normal not ok!
print("--- t53.cmd")
type human = [name::"Name" age::55]

b = human[2]("kostas",55
		  "maria", 45)
b: array 0:[b] type [AMIXED] , slot -8737970827265 type=14 rows 2,cols 2
b[1].age+=1:   1.0
b[0].age:  56.0
b[0].age*=11: 616.0
b[1].age:  46.0
"end": "end"



